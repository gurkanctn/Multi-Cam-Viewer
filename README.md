# Cpp-Projects

## Video Processing with Threads
  This code is based on a Tutorial by Javidx9 (8bits of Image Processing). In fact, the main functional changes are the following:
  
  1. The video capture function runs in a separate thread.
  2. If there's more than one webcam, pressing the TAB key switches between cameras. Up to four cameras can be used (though it's practically only limited by the number of cameras available. Who needs more than 4 cameras?). (check Issue#2)
  
  The programs main function is applying several types of image processing on the video supplied by the webcam (or an external cam).
  
  I've implemented a fresh-video constraint (bVideoBufferExists) and a provisional sleep function (to heat the cpu a bit less). It's possible that non-video-related code runs regardless of the fresh-video constraint. E.g. to read keyboard inputs, or communicate with I/Os, read files, make other animations/calculations, running AI algorithms, etc.
  
  requires:
  1. olcPixelGameEngine (available on Github)
  2. escapi (Simple Camera interface, and its dll's)
  3. a webcam of some sort (internal or external)
  
  check Javidx9's Tutorial for video processing:
  
  Relevant Video: https://youtu.be/mRM5Js3VLCk
   
  for threading I've found the Cherno's short video to be useful:
  
  https://www.youtube.com/watch?v=wXBcwHwIt_I
