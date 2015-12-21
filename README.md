trap_bug
==========


Build and Install
-----------------

Standard cmake build assuming you have geant4 installed.

    mkdir trap_bug_build
    cd trap_bug_build
    cmake ../trap_bug/. -DCMAKE_INSTALL_PREFIX=../trap_bug_run
    make install

Run the simulation

    cd ../trap_bug_run
    ./bin/ebl1  




