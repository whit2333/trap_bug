scint_test
==========


Build and Install
-----------------

Standard cmake build assuming you have geant4 installed.

    mkdir scint_test_build
    cd scint_test_build
    cmake ../scint_test/. -DCMAKE_INSTALL_PREFIX=../scint_run
    make install

Run the simulation

    cd ../scint_run
    ./bin/ebl1  

Running options
---------------



