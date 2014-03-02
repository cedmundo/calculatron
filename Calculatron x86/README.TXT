
README - 2013-23-10.

Summary

1. The project
1.1 Calculator
1.2 Kernel
1.2.1 OpenCV
1.2.2 Neocognitron
2. License
3. Issues
3.1 Fixime's
3.2 Todos's
4. Author

1. The project.

Calculatron belongs to a group of applications made for one single propouse: Calculators that can
capture input by OCR [Optical Character Recognition]. Calculatron is divided in two separated projects
called "Calculatron ARM" and "Calculatron x86", both running on Android devices, other platforms
will be supported. Each sub-project (Calculatron ARM and x86) are divided by a Calculator and a Kernel.

- Calculator part is the view and control component. 
- Kernel part is a library for control component.

*Note: The purpose of the project was to participate in the "AppLatina" (applatina.com.mx) contest.

1.1 Calculator.

Calculator doesn't need to have any specifications at all, just a Calculator that uses the 
Kernel for its task.

In the case of current version, this component have been written as fast I can and there
are so much bugs and known issues, even unfinished features. I'll work on those next year.

Calculator takes a picture, preprocess it (rotates and convert to grayscale), saves the
result bitmap and creates a pending extraction.

1.2 Kernel.

jni/Calculatron.[armeabi/x86].[h/cc]

Kernel is the mainly specification of the project, this Kernel must process a grayscale image and 
return classifications (which concordate with numbers in this case) to the calculator.

The process is divided in three steps:

1 - Binarization. OpenCV - Adaptive threshold, not operation, erode and dilate.
2 - Segmentation. OpenCV -  Canny and FindContourns.
3 - Recognition. Neocognitron - K. Fukushima.

Calculatron's Kernel is part of "com.calculatron.extraction.PendingExtraction" Java class as native 
method.

1.2.1 OpenCV.

For building Calculatron you'll need OpenCV 2.4.5 for android. Include it in the Project Includes
and configure with the default options. There are not PackageManager, this is why the project
is divided in two parts (x86 and armeabi archs): reducing file size.

1.2.2 Neocognitron.

jni/Neocognitron.[h/cc]

Neocognitron is a complex, self-organized, artificial neuronal network created by Ph. K. Fukushima.
I have only adapt its original code to my project and share as the license says.

You can find original project it here:
Paper explaining Neocognitron:

Changes for K. Fukushima project:
 - Subdivition of the single file in multiple headers.
 - Reduction of several functions [learning and stats].
 - Adaptation of the method "void pat(...)" to generate a pattern from IplImage.
 - A try to clean up memory after using it.
 
2. License.

K. Fukushima's license applies to all files in this project, including building scripts. 
but in the way to modify it you must recognize Carlos Edmundo Martínez Mendoza as author 
of Calculatron and K. Fukushima as author of Neocognitron. 

You are free:
  To copy, distribute, display, and perform the work (this computer program),
  To make derivative works,
  Under the following conditions:
  Attribution:
      You must give the original author credit.
      When you publish any of your works related to, or based on,
      this original work or derivative works,
      you must cite the reference papers listed above.
  Non-commercial:
      You may not use this work, or derivative works, for commercial purposes,
      unless you get permission from the original author of Neocognitron (K. Fukushima)
      or Calculatron (Carlos Edmundo Martínez Mendoza).
  Share Alike:
      If you alter, transform, or build upon this work,
      you may distribute the resulting work
      only under a license identical to this one.
      
For any reuse or distribution, you must make clear to others the license terms of this work.

3. Issues.

Forum: http://cedmundomartinez.com/foro/
Bug reporting: http://cedmundomartinez.com/foro/
Bug tracking: No available
 
Known Issues: 

 - Neocognitron memory free has not been fully tested, there may be memory leaks.
 - Some images are not segmented perfectly.
 - An unkown bug crash application when user captures totally black photo
 	in portrait mode. I have not correct it but appears to be gone [Important].
 - Calculator is not well designed, so some features were patched for be ready
 	and not be well done.

3.1 Fixme's

 - Correct and use "void free_D(ptr);" functions in MemoryManage.h.
 
3.2 Todo's

 - Add multi-task code for PendingExtracting class, so you can tail process bitmaps.
 - Add other activities such "Prices list", "Score list", etc.
 - Improve segmentation and binarization methods.
 
4. Author.

Name: Carlos Edmundo Martínez Mendoza.
Mail: contacto@cedmunomartinez.com
Twitter: @ced_martinez
Web-site: http://cedmundomartinez.com
