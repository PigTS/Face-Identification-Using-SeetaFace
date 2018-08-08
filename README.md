# Face-Identification-Using-SeetaFace
#### &#160; &#160; &#160; &#160;基于SeetaFace的人脸识别

&#160; &#160; &#160; &#160;SeetaFace是一个开源人脸识别解决方案，集齐了人脸识别的三部分：人脸检测、人脸对齐、人脸验证。GitHub地址为[https://github.com/seetaface/SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine)，具体安装过程可见开源库中的SeetaFace_config.docx文档，写的非常详细，网上的一些教程都是基于此文档的。

&#160; &#160; &#160; &#160;SeetaFace编译好后，得到6个文件：FaceAlignment.dll、FaceAlignment.lib、FaceDetection.lib、FaceDetection.dll、FaceIdentification.dll和FaceIdentification.lib，有了这6个文件，我们就可以基于SeetaFace搭建自己的项目，这6个文件我已经编译好并且放在lib文件夹下了(Release版本)。

&#160; &#160; &#160; &#160;代码参考[https://zhuanlan.zhihu.com/p/22605633](https://zhuanlan.zhihu.com/p/22605633)，这是一个明星脸相似度测试demo，意思就是输入一张图片，看这张图片和哪个明星最相似。

&#160; &#160; &#160; &#160;如果自己的人脸数据集不多，可以加入一些明星人脸以增大数据集，链接：https://pan.baidu.com/s/1GLI1pr3WMqTAO0X8I8vIrA 密码：9bdq（来源：何之源，上一个知乎链接），也可以自己使用爬虫在网上多爬取一些图片。数据集放入images文件夹中，以供后续提取特征。提取的人脸特征默认放入feature_model下，可以自己更改放置目录。

&#160; &#160; &#160; &#160;SeetaFace的三个模型(人脸检测/人脸对齐/人脸验证)放入model文件夹中，附上我自己的百度云链接：https://pan.baidu.com/s/13RwsuZLqSMXUx6vnVuQKBA 密码：fo3o。

&#160; &#160; &#160; &#160;images中放好人脸图片，model中放好三个模型后，再将opencv环境配置一下，就可以运行程序了，主程序为facetest/SeetaFace.cpp，运行前要将facetest/test.cpp从项目中排除。

&#160; &#160; &#160; &#160;**·** 选1，根据images中的图片创建人脸特征库。

&#160; &#160; &#160; &#160;**·** 选2，本地输入图片进行人脸比对，找出top10，从高往低打印。

&#160; &#160; &#160; &#160;**·** 选3，直接调用摄像头进行实时识别，显示数据库中和你最像的三个人，并且显示相似值。

&#160; &#160; &#160; &#160;以明星人脸数据库做示例，显示和摄像头捕捉到的人脸最像的三个明星，可以自己添加人脸放进images中做人脸识别，摄像头实时识别示例如下图：

![image](https://github.com/HuiZhou-xmu/Face-Identification-Using-SeetaFace/raw/master/example/seeta_face_demo.png)

&#160; &#160; &#160; &#160;最左边的是摄像头实时捕捉画面，右边依次是和摄像头检测到的人脸相似性值排前三的明星，库中没有本人的照片，最像的明星相似性值在0.5+。

&#160; &#160; &#160; &#160;如果库中有本人的正脸照片，和摄像头检测到的人脸对比，如果检测到的是正脸，相似性会在0.8左右，如果是侧脸/低头/抬头等(幅度不大的情况下)，相似性会在0.6~0.7左右。

&#160; &#160; &#160; &#160;++有些目录中有.gitxxx文件，是为了上传github时保留这些本来为空的文件夹，程序运行时若有错误发生，建议删除。++






