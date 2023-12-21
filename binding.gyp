{
    "targets":[
        {
            "target_name": "hello",
            "sources":["src/binding.cc"],
            "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")","src/StoveSDK/include"],
            "libraries":["<(module_root_dir)/src/StoveSDK/bin/StovePCSDK.lib"],
            'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        }
    ]
}