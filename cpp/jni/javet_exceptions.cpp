/*
 *   Copyright (c) 2021 caoccao.com Sam Cao
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "javet_converter.h"
#include "javet_exceptions.h"
#include "javet_logging.h"

namespace Javet {
	namespace Exceptions {
		void Initialize(JNIEnv* jniEnv) {
			/*
			 @see https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
			 @see https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/functions.html
			*/

			jclassJavetCompilationException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetCompilationException"));
			jmethodIDJavetCompilationExceptionConstructor = jniEnv->GetMethodID(jclassJavetCompilationException, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIII)V");
			jclassJavetConverterException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetConverterException"));
			jclassJavetExecutionException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetExecutionException"));
			jmethodIDJavetExecutionExceptionConstructor = jniEnv->GetMethodID(jclassJavetExecutionException, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIII)V");
			jclassJavetTerminatedException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetTerminatedException"));
			jmethodIDJavetTerminatedExceptionConstructor = jniEnv->GetMethodID(jclassJavetTerminatedException, "<init>", "(Z)V");
			jclassJavetUnknownCompilationException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetUnknownCompilationException"));
			jmethodIDJavetUnknownCompilationExceptionConstructor = jniEnv->GetMethodID(jclassJavetUnknownCompilationException, "<init>", "(Ljava/lang/String;)V");
			jclassJavetUnknownExecutionException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetUnknownExecutionException"));
			jmethodIDJavetUnknownExecutionExceptionConstructor = jniEnv->GetMethodID(jclassJavetUnknownExecutionException, "<init>", "(Ljava/lang/String;)V");
			jclassJavetV8LockConflictException = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("com/caoccao/javet/exceptions/JavetV8LockConflictException"));
		}

		void ThrowJavetCompilationException(JNIEnv* jniEnv, const V8LocalContext& v8Context, const V8TryCatch& v8TryCatch) {
			LOG_ERROR("Compilation exception.");
			auto isolate = v8Context->GetIsolate();
			jstring jStringExceptionMessage = Javet::Converter::ToJavaString(jniEnv, v8Context, v8TryCatch.Exception());
			auto v8LocalMessage = v8TryCatch.Message();
			if (v8LocalMessage.IsEmpty()) {
				jthrowable javetUnknownCompilationException = (jthrowable)jniEnv->NewObject(
					jclassJavetUnknownCompilationException,
					jmethodIDJavetUnknownCompilationExceptionConstructor,
					jStringExceptionMessage);
				jniEnv->Throw(javetUnknownCompilationException);
			}
			else {
				jstring jStringScriptResourceName = Javet::Converter::ToJavaString(jniEnv, v8Context, v8LocalMessage->GetScriptResourceName());
				jstring jStringSourceLine = Javet::Converter::ToJavaString(jniEnv, v8Context, v8LocalMessage->GetSourceLine(v8Context).ToLocalChecked());
				jthrowable javetConverterException = (jthrowable)jniEnv->NewObject(
					jclassJavetCompilationException,
					jmethodIDJavetCompilationExceptionConstructor,
					jStringExceptionMessage,
					jStringScriptResourceName,
					jStringSourceLine,
					v8LocalMessage->GetLineNumber(v8Context).FromMaybe(0),
					v8LocalMessage->GetStartColumn(),
					v8LocalMessage->GetEndColumn(),
					v8LocalMessage->GetStartPosition(),
					v8LocalMessage->GetEndPosition());
				jniEnv->Throw(javetConverterException);
				jniEnv->DeleteLocalRef(jStringSourceLine);
				jniEnv->DeleteLocalRef(jStringScriptResourceName);
			}
			jniEnv->DeleteLocalRef(jStringExceptionMessage);
		}

		void ThrowJavetConverterException(JNIEnv* jniEnv, const char* message) {
			LOG_ERROR(*message);
			jniEnv->ThrowNew(jclassJavetConverterException, message);
		}

		void ThrowJavetExecutionException(JNIEnv* jniEnv, const V8LocalContext& v8Context, const V8TryCatch& v8TryCatch) {
			auto isolate = v8Context->GetIsolate();
			if (v8TryCatch.HasTerminated()) {
				LOG_ERROR("Execution has been terminated.");
				jthrowable javetTerminatedException = (jthrowable)jniEnv->NewObject(
					jclassJavetTerminatedException,
					jmethodIDJavetTerminatedExceptionConstructor,
					v8TryCatch.CanContinue());
				jniEnv->Throw(javetTerminatedException);
			}
			else {
				LOG_ERROR("Execution exception.");
				jstring jStringExceptionMessage = Javet::Converter::ToJavaString(jniEnv, v8Context, v8TryCatch.Exception());
				auto v8LocalMessage = v8TryCatch.Message();
				if (v8LocalMessage.IsEmpty()) {
					jthrowable javetUnknownExecutionException = (jthrowable)jniEnv->NewObject(
						jclassJavetUnknownExecutionException,
						jmethodIDJavetUnknownExecutionExceptionConstructor,
						jStringExceptionMessage);
					jniEnv->Throw(javetUnknownExecutionException);
				}
				else {
					jstring jStringScriptResourceName = Javet::Converter::ToJavaString(jniEnv, v8Context, v8LocalMessage->GetScriptResourceName());
					jstring jStringSourceLine = Javet::Converter::ToJavaString(jniEnv, v8Context, v8LocalMessage->GetSourceLine(v8Context).ToLocalChecked());
					jthrowable javetConverterException = (jthrowable)jniEnv->NewObject(
						jclassJavetExecutionException,
						jmethodIDJavetExecutionExceptionConstructor,
						jStringExceptionMessage,
						jStringScriptResourceName,
						jStringSourceLine,
						v8LocalMessage->GetLineNumber(v8Context).FromMaybe(0),
						v8LocalMessage->GetStartColumn(),
						v8LocalMessage->GetEndColumn(),
						v8LocalMessage->GetStartPosition(),
						v8LocalMessage->GetEndPosition());
					jniEnv->Throw(javetConverterException);
					jniEnv->DeleteLocalRef(jStringSourceLine);
					jniEnv->DeleteLocalRef(jStringScriptResourceName);
				}
				jniEnv->DeleteLocalRef(jStringExceptionMessage);
			}
		}

		void ThrowJavetV8LockConflictException(JNIEnv* jniEnv, const char* message) {
			jniEnv->ThrowNew(jclassJavetV8LockConflictException, message);
		}
	}
}
