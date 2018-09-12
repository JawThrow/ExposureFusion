## ExposureFusion
### Introduction

This is the implementation of Exposure fusion algorithm ('Exposure fusion' In proceedings of Pacific Graphics 2007).
I coded this algorithm on my own using C++ and OpenCV.
If you have any theoretical question about Exposure fusion, i recommend you to read the paper or contact author.

Since I'm not a professional but just a student, I knowledge that there are some mistakes and unefficiency in my code.
I always welcome your feedback.

I will add description and some figures of Exposure fusion soon.

### Description
Exposure fusion is a useful technique to make a high quality image from bracket images, that is multiple exposure images from identical scene. Bracket images are kinds of special image sequences that are captured from same scene with different exposure options(shutter speed and size of aperture), so that these images have different well-exposed or saturated region like below figure.

![image](https://user-images.githubusercontent.com/36951642/38173271-60ba8972-35f6-11e8-9a58-87374d33973d.png)

Since left image(LDR image1) was captured at a short shutter speed, outdoor region was well expressed, but indoor region was under-saturated. On the contrary, middle image(LDR image2) was captured at a long shutter speed, so indoor region was well represented but outdoor region was over-saturated. Exposure fusion combines the well saturated/exposed regions in each LDR images and makes one result image like right image that expresses both region well!

(I will add additional description)

From [Wikipedia](https://en.wikipedia.org/wiki/Exposure_fusion)...
| --- |
|In image processing, computer graphics, and photography, exposure fusion is a technique for blending multiple exposures of the same scene into a single image. As in high dynamic range imaging (HDRI or just HDR), the goal is to capture a scene with a higher dynamic range than the camera is capable of capturing with a single exposure. However, because no HDR image is ever created during exposure fusion, it cannot be considered an HDR technique.|

### Demo video
This is a demo video of my program based on modified exposure fusion. Although theorical and practical elements are slightly different from Exposure fusion, the process to make a result image is identical(synthesize a high-qaulity image from braket images). 

[![Video Label](https://img.youtube.com/vi/43N2rdNbWIs/0.jpg](https://youtu.be/43N2rdNbWIs?t=0s)

### Contents
| Folder | description |
| --- | --- |
|data|test image sequences(multiple exposure images, braket exposure image), most of them are from [here](add reference paper and url)|
|source|C++ source code|



