pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
        stage('Build macOS Debug') {
          agent {
            node {
              label 'macos_dbg'
            }
          }
          steps {
            sleep 10
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"

               # Get dependencies (we force it on macOS/Linux because of CMake/Ninja issue)
               mkdir -p build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
            '''
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"
    
               # Build Debug
               mkdir -p build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_DBG=1
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build macOS') {
          agent {
            node {
              label 'macos'
            }
          }
          steps {
            sleep 10
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"

               # Get dependencies (we force it on macOS/Linux because of CMake/Ninja issue)
               mkdir -p build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
            '''
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"

               # Build Release
               mkdir -p build
               cd build
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Windows x64 Debug') {
          agent {
            node {
              label 'win_x64'
            }
          }
          steps {
            sleep 10
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe
              
               rem Build Debug
               if not exist build_dbg mkdir build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Windows x64') {
          agent {
            node {
              label 'win_x64'
            }
          }
          steps {
            sleep 10
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe

               rem Build Release
               if not exist build mkdir build
               cd build
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Windows x64 UWP Debug') {
          agent {
            node {
              label 'win10_x64'
            }
          }
          steps {
            sleep 10
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" amd64 uwp
               set CC=cl.exe
               set CXX=cl.exe
              
               rem Build Debug
               if not exist build_dbg mkdir build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUWP_PLATFORM=1 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Windows x64 UWP') {
          agent {
            node {
              label 'win10_x64'
            }
          }
          steps {
            sleep 10
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" amd64 uwp
               set CC=cl.exe
               set CXX=cl.exe

               rem Build Release
               if not exist build mkdir build
               cd build
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DUWP_PLATFORM=1 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Linux Debug') {
          agent {
            node {
              label 'linux_dbg'
            }
          }
          steps {
            sleep 10
            sh '''
               # Get dependencies (we force it on macOS/Linux because of CMake/Ninja issue)
               mkdir -p build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
            '''
            sh '''     
               # Build Debug
               mkdir -p build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_DBG=1
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Linux') {
          agent {
            node {
              label 'linux'
            }
          }
          steps {
            sleep 10
            sh '''
               # Get dependencies (we force it on macOS/Linux because of CMake/Ninja issue)
               mkdir -p build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
            '''
            sh '''     
               # Build Release
               mkdir -p build
               cd build
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
      }
    }
  }
}

def deployDebug() {
  withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com', credentials:'jenkins-access') {
    if (env.BRANCH_NAME == 'master') {
      s3Upload(bucket: 'webcore-bin-dbg', workingDir:'build_dbg', includePathPattern:'*.7z', acl:'PublicRead');
    } else if (env.BRANCH_NAME == 'dev') {
      s3Upload(bucket: 'webcore-bin-dev-dbg', workingDir:'build_dbg', includePathPattern:'*.7z', acl:'PublicRead');
    }
  }
}

def deploy() {
  withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com', credentials:'jenkins-access') {
    if (env.BRANCH_NAME == 'master') {
      s3Upload(bucket: 'webcore-bin', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
    } else if (env.BRANCH_NAME == 'dev') {
      s3Upload(bucket: 'webcore-bin-dev', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
    }
  }
}