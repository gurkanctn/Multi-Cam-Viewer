# Cpp-Projects

## Video Processing with Threads
  This one is based on a Tutorial by Javidx9 (8bits of Image Processing)
  The program runs several types of image processing on the video supplied by the webcam (or an external cam)
  I've implemented a fresh-video constraint (bVideoBufferExists) and a provisional sleep function (to heat the cpu a bit less), however non-video-related code can run unconditionally (e.g. to read keyboard inputs, or communicate with I/Os, etc.).
  
  requires:
  1. olcPixelGameEngine (available on Github)
  2. escapi (Simple Camera interface, and its dll's)
  3. a webcam of some sort (internal or external)
  
  check Javidx9's Tutorial for video processing
   Relevant Video: https://youtu.be/mRM5Js3VLCk
   
  for threading I've found the Cherno's short video to be useful:
  https://www.youtube.com/watch?v=wXBcwHwIt_I
