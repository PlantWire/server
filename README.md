# pWire Server

## For Users:

[Latest Build](https://gitlab.dev.ifs.hsr.ch/epj/2020/pwire/pwire-server/-/jobs/artifacts/master/raw/exe/build/pwire-server?job=build_prod)

## For Developers:

### Requirements:

pWire Server uses the following software for it's build process:

- [cpplint](https://github.com/cpplint/cpplint) for linting
- [cute](https://cute-test.com/) as testing framework
- [make](https://en.wikipedia.org/wiki/Make_(software)) as build-management tool
1. Install make on your pc
2. Install cpplint on your pc (pip can be used see link above)
3. Use a IDE to your liking (cevelop supports both makefiles and cute)

### Usage:

1. Change some code
2. Adjust tests if necessary or write new ones
3. Run tests (run in pwire-server directory)
   
   ```
    make build_test
    make run_test
   ```
4. Run cpplint (run in pwire-server directory)
   
   ```
    make lint
   ```
   
    For vscode exists an extension for cpplint which can also be used with the config:
   
   ```json
    {
        "editor.suggestSelection": "first",
        "vsintellicode.modify.editor.suggestSelection": "automaticallyOverrodeDefaultValue",
        "java.errors.incompleteClasspath.severity": "ignore",
        "cpplint.filters": [
            "-legal/copyright"
        ],
        "cpplint.excludes": ["test/cute", ".metadata"]
    }
   ```
5. Upload to repo (only if 3. and 4. had no errors)
