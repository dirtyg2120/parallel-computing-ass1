# Parallel Computing Assignment 1


Assignment description: [Assignment1_v3_18022025.pdf](https://lms.hcmut.edu.vn/pluginfile.php/1176202/mod_resource/content/1/Assignment1_v3_18022025.pdf)

## Prerequisites

1. **SSH Public Key**: In order to access the server, you need to provide your SSH public key.
   
   ### Generating SSH Key (Windows/Linux)

   1. Open your terminal (Command Prompt or Git Bash for Windows, Terminal for Linux).
   
   2. Run the following command to generate the SSH keys:

      ```bash
      ssh-keygen
      ```

      - Press `Enter` to skip all the questions.
      - This will generate two files: `id_ed25519` and `id_ed25519.pub`.

   3. Now, copy the content of the `id_ed25519.pub` file and send it to me (your SSH public key).

2. **Access to Server**: Once your key is added to the server, you can SSH into the server using:

   ```bash
   ssh root@<server_ip>
   ```

   You’ll have access to the **playground** where you can work on your scripts.

## Running MPI Processes

1. **Install MPI**: First, you need to install MPI3 related libraries on your system.

   ```bash
   sudo apt update
   sudo apt install openmpi-bin openmpi-common libopenmpi-dev
   ```
   
3. **Setup Hostfile**: To run MPI across multiple nodes, create a file named `hostfile` in your working directory with the following content:

   ```bash
   localhost slots=2
   root@128.199.71.102 slots=2
   ```

   - The `localhost slots=2` means you’re requesting two processors on your local machine.
   - The `128.199.107.103 slots=2` line is **optional**. Remove it to run on the local machine only. 
   - The `slots` indicate the number of processors you want to allocate for your MPI job.

4. **Running the Script**: To compile and run the MPI program, follow these steps:

   ### Compile the C++ Script

   ```bash
   mpic++ -o srb_mpi srb_mpi.cpp
   ```

   ### Run the MPI Process

   ```bash
   mpirun -np 4 --hostfile hostfile ./srb_mpi
   ```

   - `-np 4` specifies that 4 processes should be used.
   - You can adjust the number of processes (`4` in this example) based on your system and requirements.
   
## For MutexLock Execution
   1. Simply, build and run the MutexLock.sln with Visual Studio. It will start testing and logging for serveral configuration for each locks
   2. After finish, it will provide 2 files as output: performance-results.json and correctness-results.json
   3. The process may take a long time. So, I've prepare a sample in folder parallel-computing-ass1\mutex-locks\output-samples
   4. You can use the output files together with .ipynb file in parallel-computing-ass1\mutex-locks\visualization-ipynb to visualize the result
