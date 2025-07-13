#项目描述
本项目是用于智能车比赛，通过视觉巡线

#工程结构
.
├── guide_node--用于存放项目使用到的第三方库的说明文档
├── include
│   ├── common--用于存放通用代码，如简单的图像处理、数学运算等
│   ├── detection--用于存放检测代码，如车道线检测、障碍物检测等
│   └── recognition--用于存放AI识别代码，如标志物识别、人脸识别等
├── libs--用于存放第三方库
├── res--用于存放资源文件，如图片、视频等,用于调试
└── src
    ├── common--用于存放通用代码，如简单的图像处理、数学运算等
    ├── detection--用于存放检测代码，如车道线检测、障碍物识别等
    └── recognition--用于存放识别代码，如车牌识别、人脸识别等

#项目依赖
- OpenCV
- nlohmann_json
- serilib


git remote add origin https://github.com/Water-Canoe/Natural_Selection_ByLYH.git
git branch -M main
git push -u origin main


# 1. 添加所有更改到暂存区
git add .

# 2. 提交更改（添加有意义的提交信息）
git commit -m "你的提交信息"

# 3. 推送到GitHub
git push origin main



#debug_mode
1. picture  并且在picture_path配置好图片路径
2. video    main.cpp里，
3. camera 实际执行摄像头

#调参
