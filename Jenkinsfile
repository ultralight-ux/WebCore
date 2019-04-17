pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
        /*
        stage('Build macOS') {
          agent {
            node {
              label 'macos'
            }
          }
          steps {
            sh '''
               mkdir -p build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
               mkdir -p build
               cd build
               cmake .. -G "Ninja"
               ninja
               cd ..
            '''
          }
        }
        */
        stage('Build Windows x64') {
          agent {
            node {
              label 'windows'
            }
          }
          steps {
            bat '''
               if not exist build_deps mkdir build_deps
               cd build_deps
               cmake ../Source/GetDeps -G "Ninja"
               ninja
               cd ..
               if not exist build mkdir build
               cd build
               call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe
               cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel
               ninja
               cd ..
            '''
          }
        }
      }
    }
    stage('Deploy') {
      /*
      when {
        branch 'dev'
      }
      */
      steps {
        withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com',credentials:'jenkins-access') {
          s3Delete(bucket: 'bin', path:'**/*')
          s3Upload(bucket: 'bin', workingDir:'build', includePathPattern:'**/*');
        }
      }
    }
  }
}