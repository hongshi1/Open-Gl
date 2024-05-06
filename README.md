
### 1. Run

执行命令

```Bash
make.exe run dir=bloom
```

### 2. Debug

需要设置调试一个文件 修改 `tasks.json`下面的条目

```js
 "label": "build",
  "type": "shell",
  "group": {
      "kind": "build",
      "isDefault": true
  },
  "windows": {
      "command": "powershell",
      "args": [
          "-c",
          "make dir=01_test_glfw", // 调式 01_test_glfw/main.cpp
      ]
  },
```
