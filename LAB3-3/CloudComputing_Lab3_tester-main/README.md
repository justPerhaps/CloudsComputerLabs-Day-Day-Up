<html>
<center>
<h1>
Lab3 Tester
</h1></center>
</html>

# Usage of test scripts

`./lab3_testing.sh [source_folder] [your_sudo_password] [result_folder] [version]`

For example:
`./lab3_testing.sh ./Lab3 XXXXXX ./Lab3 1`

You need to pass 4 arguments (the lab3 path and your sudo password, result_folder is a existing folder to store testing result, version refers to whether you are implementing a basic or advanced version) to the script, **and that's enough**. 

***Why need sudo privilege:***

For the sake of convenience, we will create several virtual NICs and set some parameters (such as delay, packet loss rate, etc), and these operations require sudo privilege. Ease up, there have no 'dangerous' operations in this script.

***How do you know your score:***

When the test script finishes running, it will be displayed at the bottom. The execution of the test script may take a few minutes, as it is necessary to wait for the service to restart at the fault-tolerant testing point. Therefore, please be patient and wait.

```shell
Language: [ C/C++ ]
VERSION: [ 1 ]
----- Passing situation -------
Test items 1 [ PASSED ]
Test items 2 [ PASSED ]
Test items 3 [ PASSED ]
Test items 4 [ PASSED ]
Test items 5 [ PASSED ]
Test items 6 [ PASSED ]
Test items 7 [ PASSED ]
Test items 8 [ PASSED ]
Test items 9 [ PASSED ]
Test items 10 [ PASSED ]
----- Total score: [ 18 ] ------
```

***NOTE:***

- **Naming of execution files:** If you are implementing a storage system using the 2pc protocol, please configure your executable file as **kvstore2pcsystem**; If you are implementing a storage system using the raft protocol, please name your executable file **kvstoreraftsystem**.

- **Programming Language:** The testing script will automatically detect the programming language you are using. Currently, the testing script can only detect C/C++, Java, and Python. If you use other programming languages, the default execution method will be the same as C/C++.

- **Configuration files:** You need to configure the startup configuration file as described in the laboratory guide, otherwise it will cause the test to fail.
  
  - Sample coordinator configuration file(**basic**):
    
    ```tsconfig
    !
    ! Coordinator configuration
    !      2023/05/05 11:25:33
    !
    ! The argument name and value are separated by whitespace in the configuration file.
    !
    ! Mode of process, coordinator OR participant
    mode coordinator
    !
    ! The address and port the coordinator process is listening on.
    ! Note that the address and port are separated by character ':'. 
    coordinator_info 127.0.0.1:8001
    !
    ! Address and port information of all participants. 
    ! Three lines specifies three participants' addresses.
    participant_info 127.0.0.1:8002 
    participant_info 127.0.0.1:8003 
    participant_info 127.0.0.1:8004
    ```
  
  - Sample participant configuration file(**basic**):
    
    ```tsconfig
    !
    ! Participant configuration
    !      2023/05/05 11:25:33
    !
    ! The argument name and value are separated by whitespace in the configuration file.
    !
    ! Mode of process, coordinator OR participant
    mode participant
    !
    ! The address and port the participant process is listening on.
    participant_info 127.0.0.1:8002
    !
    ! The address and port the coordinator process is listening on.
    coordinator_info 127.0.0.1:8001
    ```
  
  - Sample configuration file implemented with raft(**advanced**):
    
    ```tsconfig
    !
    ! configuration of advanced version
    !      2023/05/05 11:25:33
    !
    ! The argument name and value are separated by whitespace in the configuration file.
    !
    ! The address and port the follower process is listening on.
    follower_info 127.0.0.1:8001
    !
    ! The address and port the other follower processes are listening on.
    follower_info 127.0.0.1:8002
    follower_info 127.0.0.1:8003
    ```

- **For advanced version:** If you implement a storage system using the Raft protocol, the testing script will not know who the leader is at the beginning and will default to configuring the first follower as the leader (i.e., sending messages to this process). If this process is not the leader, the request will be ignored, and the leader's information will be returned in the format "<ip>:<port>", such as "127.0.0.1:8001".

# Scoring criteria

Please carefully read the [implementation requirements](https://github.com/LabCloudComputing/CloudComputingLabs_22/tree/2022/Lab3#4-implementation-requirements) provided in the lab3 guide book. The specific testing items for each scoring point will be provided below.

## Basic Version(18 points totally)

### 1st level of basic version(15 points)

- Test item 1: Run kvstoresystem.

- Test item 2: Set key to hold string value.

- Test item 3: Get the value of the key.

- Test item 4: Return nil if the key does not exist.

- Test item 5: If the DEL command executed, return the number of keys that were removed.

- Test item 6: When associating a new value to an existing key, it should overwrite the value of the existing entry

- Test item 7: Correctness testing of DEL command.

The above testing items are all functional tests aimed at testing whether your KV storage system can correctly complete the implementation tasks given in the lab3 guide book.

### 2nd level of basic version(16 points)

- Test item 8: Kill one of the storage process, execute SET and GET behaviors again.

- Test item 9: Kill all of the storage processes, execute SET and GET behavior again.

The above testing items aim to test the fault-tolerant ability of the KV storage system you have implemented. We kill different numbers of storage processes and request the system to rate you based on the returned results

### 3rd level of basic version(18 points)

- Test item 10: Kill certain numbers of the storage processes, then restart the killed storage processes, execute SET, GET and DEL behaviors again.

The above test item is designed to test the robustness of the KV storage system you have implemented. By killing a certain number of storage processes and restarting (there will be 10 seconds after restarting, so that the restarted service can copy the data stored by other alive services), the teaching assistant will score according to the returned results.

## Advanced Version(20 points)

You have implemented a kV storage system using raft and passed all the above test points.
