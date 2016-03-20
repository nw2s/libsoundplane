Soundplane lite library


building linux 

    mkdir build
    cd build
    cmake .. 

Mac (with xcode)

    mkdir build
    cd build
    cmake .. -GXcode 
    xcodebuild -project mec.xcodeproj -target ALL_BUILD -configuration MinSizeRel



Mac OSX 10.11 - Stopping apps
becareful when stopping under El Capitan (MAC OSX 10.11) , abrutly stopping usb drivers will cause panic
for mec use osc to terminate EVEN inside Xcode!

    oscsend localhost 9000 /tb/mec/command s stop


(there is also a Ctrl-C handler which will shutdown nicely)






