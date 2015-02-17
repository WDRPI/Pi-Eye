# Pi-Eye
This project is a real time eye tracking system that runs on the Raspberry Pi designed to be implemented as an alternative to other peripherals on standard computers. This project also contains source files for utilities made and used to construct weak and strong classifieds for this algorithm from existing facial data. 

The peripherals used to create and modify the data for Pi-Eye are as follows:

•	Image Normalizer (C#): This Program was created to easily collect and create the data we needed for the project. It would would normalize the image as well as scale it to 24x24 resolutions as well giving the option to split images into random
chunks(used to create the Not-face images). It then saves the data into files together with structured names to allow easy use.

•	Integral Image Creator (Java): This took the now grouped images and created the Integral Images related to each one to allow for faster access by the C++ feature creator as Java could readily read and edit images.

•	Feature Calculator (C++): This take the Integral Images calculated previously and iterates through every one of the 160,000+ haar features possible, finding and saving the best values for each in a CSV for which we can analyse in excel. 

•	Feature Blacklister (Java): This program simply visually displays each feature on a template face so that we can decide if it is genuine or not, this is needed as some features may really on the corner of the screen being black for example which is not dependent on the face.

•	Eye Detector (C++ on Raspberry Pi): This Program is run on the pi an works by taking the best features created by the previous programs and using them to detect the face of the user, once it is found it finds areas that are most likely eyes and based of frequency and position determines weather or not it is an actual eye, from there it relays it via a serial to USB cable to the final program.

•	Pi Interface Program (C#): This program takes advantage of c#’s ability to interface with serial to take the information the pi has sent and turn it into mouse movement as well as an embedded keyboard to allow typing from the mouse.
