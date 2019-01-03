In order to run the ftserver:
1. Navigate to the ftserver folder in the terminal
2. type make
3. type ftserver <port #>

In order to run the ftclient:
1. Navigate to the ftclient folder in the terminal
2a. To get the directory inventory type python3 ftclient.py <server address> <server port #> <-l> <data port #>
2b. To copy a file type python3 ftclient.py <server address> <server port #> <-g> <filename> <data port #>