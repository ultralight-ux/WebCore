pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
        stage('Build macOS') {
          agent {
            node {
              label 'mac_x64'
            }

          }
          steps {
            sh '''
               cmake --version
               which cmake
               ninja --version
               which ninja
               mkdir -p build
               cd build
               cmake .. -G "Ninja"
               ninja
               cd ..
            '''
          }
        }
      }
    }
  }
}