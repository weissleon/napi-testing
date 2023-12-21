const addon = require("./build/Release/hello.node");
const process = require("process");

const run = () => {
  process.stdin.resume();

  const config = {
    env: "Live",
    appKey: "ea3f7f50505a14b63fb92c471ad72fe585e86bbb7a5b613557c7cb7ec0322f79",
    secretKey:
      "472e3e37abcfd60116ed3cb03987b2d684c294d2df97778fbe73de0ba48e4f8a38da4ab347571218bde8fccd582e58b0",
    gameId: "lovein_login",
    logLevel: 4,
    logPath: "",
  };

  const callback = {
    onError: (error) => {},
    onInitComplete: () => {
      console.log(`Get User Result: ${addon.getUser()}`);
    },
    onUser: (user) => {
      console.log(user);
    },
    onOwnership: () => {},
  };

  const result = addon.init(config, callback);
  console.log(`Init result: ${result}`);

  if (result === 0) {
    setInterval(() => {
      const response = addon.runCallback();
    }, 10);
  }

  //   setInterval(() => {
  //     const response = addon.getInitState();
  //     console.log(response);
  //   }, 10);

  //   addon.uninit();
};

run();
