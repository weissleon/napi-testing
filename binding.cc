#include "napi.h"
#include "StovePCSDK.h"

Napi::FunctionReference OnError;
Napi::FunctionReference OnInitComplete;
Napi::FunctionReference OnToken;
Napi::FunctionReference OnUser;
Napi::FunctionReference OnOwnership;

Napi::Env globalEnv = nullptr;

void HandleOnError(const StovePCError error)
{
    // OnError.IsEmpty
}
void HandleOnInitComplete()
{
    OnInitComplete.Call({});
}
void HandleOnToken(const StovePCToken token)
{
}
void HandleOnUser(const StovePCUser user)
{
    Napi::Object payload = Napi::Object::New(globalEnv);
    Napi::String nickname = Napi::String::New(globalEnv, reinterpret_cast<const char16_t *>(user.nickname));
    payload.Set(Napi::String::New(globalEnv, "memberNo"), Napi::Number::New(globalEnv, user.memberNo));
    payload.Set(Napi::String::New(globalEnv, "nickname"), nickname);
    payload.Set(Napi::String::New(globalEnv, "gameUserId"), Napi::String::New(globalEnv, user.gameUserId));

    OnUser.Call({payload});
}
void HandleOnOwnership(int size, StovePCOwnership *ownership) {}

Napi::Number Init(Napi::CallbackInfo &info)
{
    Napi::Env jsEnv = info.Env();
    Napi::Object jsConfig = info[0].As<Napi::Object>();

    std::string env = jsConfig.Get("env").As<Napi::String>().Utf8Value();
    std::string appKey = jsConfig.Get("appKey").As<Napi::String>().Utf8Value();
    std::string secretKey = jsConfig.Get("secretKey").As<Napi::String>().Utf8Value();
    std::string gameId = jsConfig.Get("gameId").As<Napi::String>().Utf8Value();
    std::int32_t logLevel = jsConfig.Get("logLevel").As<Napi::Number>().Int32Value();
    std::string logPath = jsConfig.Get("logPath").As<Napi::String>().Utf8Value();

    Napi::Object jsCallback = info[1].As<Napi::Object>();

    globalEnv = jsEnv;
    OnInitComplete = Napi::Persistent(jsCallback.Get("onInitComplete").As<Napi::Function>());
    OnUser = Napi::Persistent(jsCallback.Get("onUser").As<Napi::Function>());

    std::wstring gameIdW;
    gameIdW.assign(gameId.begin(), gameId.end());
    std::wstring logPathW;
    logPath.assign(logPath.begin(), logPath.end());
    StovePCLogLevel logLevelE = static_cast<StovePCLogLevel>(logLevel);

    StovePCConfig config{
        env.data(),
        appKey.data(),
        secretKey.data(),
        gameIdW.data(),
        logLevelE,
        logPathW.data()};

    StovePCCallback callback;

    memset(&callback, 0, sizeof(StovePCCallback));

    callback.OnError = HandleOnError;
    // callback.OnInitComplete = []
    // { OnInitComplete.Call(globalEnv.Global(), {Napi::String::New(globalEnv, "Hi")}); };
    callback.OnInitComplete = HandleOnInitComplete;
    callback.OnToken = HandleOnToken;
    callback.OnUser = HandleOnUser;
    callback.OnOwnership = HandleOnOwnership;

    StovePCResult result = StovePC_Init(config, callback);

    return Napi::Number::New(jsEnv, result);
}

Napi::Number RunCallback(Napi::CallbackInfo &info)
{
    Napi::Env jsEnv = info.Env();
    StovePCResult result = StovePC_RunCallback();

    return Napi::Number::New(jsEnv, result);
}

void Uninit(Napi::CallbackInfo &info)
{
    StovePC_UnInit();
}

Napi::Number GetInitState(Napi::CallbackInfo &info)
{
    Napi::Env jsEnv = info.Env();
    return Napi::Number::New(jsEnv, StovePC_GetInitState());
}

Napi::Number GetUser(Napi::CallbackInfo &info)
{
    Napi::Env jsEnv = info.Env();
    return Napi::Number::New(jsEnv, StovePC_GetUser());
}

Napi::Object InitModule(Napi::Env env, Napi::Object exports)
{

    exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, Init));
    exports.Set(Napi::String::New(env, "runCallback"), Napi::Function::New(env, RunCallback));
    exports.Set(Napi::String::New(env, "getInitState"), Napi::Function::New(env, GetInitState));
    exports.Set(Napi::String::New(env, "getUser"), Napi::Function::New(env, GetUser));
    exports.Set(Napi::String::New(env, "uninit"), Napi::Function::New(env, Uninit));

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitModule)