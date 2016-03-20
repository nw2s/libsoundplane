###############################
# MEC app
set(MEC_SRC 
    mec.h
    mec.cpp
    osc_cmd.cpp
    #eigen_cmd.cpp
    soundplane_cmd.cpp
)
include_directories (
	#"${PROJECT_SOURCE_DIR}/eigenharp" 
	"${PROJECT_SOURCE_DIR}/external/oscpack"
	"${PROJECT_SOURCE_DIR}/external/portaudio"
    "${PROJECT_SOURCE_DIR}/soundplanelite"
#    "${PROJECT_SOURCE_DIR}/soundplane/SoundplaneLib"
#    "${PROJECT_SOURCE_DIR}/soundplane/madronalib"
#    "${PROJECT_SOURCE_DIR}/soundplane/touchtrackerlib"
	)

add_executable(mec ${MEC_SRC})


#target_link_libraries (mec eigenharplib soundplanelite oscpack portaudio)
target_link_libraries (mec soundplanelite oscpack portaudio)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#  target_link_libraries(soundplanelib  "${PROJECT_SOURCE_DIR}/external/libatomic_gcc7/libatomic.a")
  target_link_libraries(mec atomic)
endif()

if(APPLE)
target_link_libraries(mec  "-framework CoreServices -framework CoreFoundation -framework IOKit -framework CoreAudio")
elseif(UNIX) 
target_link_libraries(mec  "libusb" "dl" "pthread")
endif(APPLE)
