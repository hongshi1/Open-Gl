
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

## 场景部分说明
    wasd 对应移动
    按下ctrl的时候进入上帝模式,w 和s 可以飞行移动 不按的话则对应水平方向移动
    z 对应开关灯
    q 降低亮度
    e提高亮度
    在普通模式 非上帝模式下 按下空格进行跳跃(模拟了重力加速度完成了跳跃)
    按下shift 进入加速冲刺移动


## 模型部分说明
### 模型一 方块 葛宣成
  这个是模型对应的实例说明,这个方块我抄的教程的,每个人在文档中更新自己模型的使用和一些点