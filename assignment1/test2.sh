#!/bin/bash

#globals
ret=0
server="server"
client="client"
serverOut=testServerOutput.txt
clientOut=testClientOutput.txt
successFile=testSuccess.txt
IPADDRESS=localhost
PORT=2000

# --- helper function ---
function run(){
    echo -e "$1:"
    $1 #execute
    #check errors
    tmp=$?
    if [ $tmp -ne 0 ]; then
        ret=$tmp
    fi
    echo "" #newline
    return $tmp
}

function checkConnection() {
    sleep 0.1
    case `netstat -a -n -p 2>/dev/null | grep $PORT` in
    *":$PORT"*"LISTEN"*"server"*)
        return 1;;
    esac
    sleep 0.2
    case `netstat -a -n -p 2>/dev/null | grep $PORT` in
    *":$PORT"*"LISTEN"*"server"*)
        return 1;;
    esac
    sleep 0.2
    case `netstat -a -n -p 2>/dev/null | grep $PORT` in
    *":$PORT"*"LISTEN"*"server"*)
        return 1;;
    esac
    return 0
}

# --- TESTCASES ---
function interactive_testcase() {
    t="interactive test case"
    # cleanup
    rm -f $serverOut
    rm -f $successFile
    echo "Rule added" > $successFile

    if  [ ! -x ./$server ] ; then
        echo -e "ERROR: Server program not found"
        return -1
    fi
    
    killall $server > /dev/null 2> /dev/null
    # start server
    echo -en "starting server: \t"
    echo "A 147.188.193.15 22" | ./$server -i > $serverOut  2>&1 &
    if [ $? -ne 0 ]
    then
        echo -e "ERROR: could not start interactive server"
        return -1
    else
        echo "OK"
    fi
    sleep 0.5
    killall $server > /dev/null 2> /dev/null
    echo -en "server result:     \t"
    res=`diff $serverOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
}

function interactive_add_rule_testcase(){
    t="interactive add rule test case"
    # cleanup
    rm -f $serverOut
    rm -f $successFile
    echo "Rule added" > $successFile

    if  [ ! -x ./$server ] ; then
        echo -e "ERROR: Server program not found"
        return -1
    fi
    
    killall $server > /dev/null 2> /dev/null
    # start server
    echo -en "starting server: \t"
    echo "A 192.168.1.1-192.168.1.10 300-400" | ./$server -i > $serverOut  2>&1 &
    if [ $? -ne 0 ]
    then
        echo -e "ERROR: could not start interactive server"
        return -1
    else
        echo "OK"
    fi
    sleep 0.5
    killall $server > /dev/null 2> /dev/null
    echo -en "server result:     \t"
    res=`diff $serverOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
}

function interactive_query_rule_testcase(){
    t="interactive query rule test case"
    # cleanup
    rm -f $serverOut
    rm -f $successFile
    echo "Connection rejected" > $successFile

    if  [ ! -x ./$server ] ; then
        echo -e "ERROR: Server program not found"
        return -1
    fi
    
    killall $server > /dev/null 2> /dev/null
    # start server
    echo -en "starting server: \t"
    echo "C 192.168.1.2 350" | ./$server -i > $serverOut  2>&1 &
    if [ $? -ne 0 ]
    then
        echo -e "ERROR: could not start interactive server"
        return -1
    else
        echo "OK"
    fi
    sleep 0.5
    killall $server > /dev/null 2> /dev/null
    echo -en "server result:     \t"
    res=`diff $serverOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
}

function interactive_delete_rule_testcase(){
    t="interactive delete rule test case"
    # cleanup
    rm -f $serverOut
    rm -f $successFile
    echo "Rule invalid" > $successFile

    if  [ ! -x ./$server ] ; then
        echo -e "ERROR: Server program not found"
        return -1
    fi
    
    killall $server > /dev/null 2> /dev/null
    # start server
    echo -en "starting server: \t"
    echo "D 192.168.1.1-192.168.1.10 300-400" | ./$server -i > $serverOut  2>&1 &
    if [ $? -ne 0 ]
    then
        echo -e "ERROR: could not start interactive server"
        return -1
    else
        echo "OK"
    fi
    sleep 0.5
    killall $server > /dev/null 2> /dev/null
    echo -en "server result:     \t"
    res=`diff $serverOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
}

function basic_testcase(){
    t="testcase 1"
    #cleanup
    rm -f $serverOut
    rm -f $clientOut
    rm -f $successFile
    echo "Rule added" > $successFile
    killall $server > /dev/null 2> /dev/null

    # start server
    echo -en "starting server: \t"
    ./$server $PORT > $serverOut  2>&1 &
    checkConnection
    if [ $? -ne 1 ]
    then
        echo -e "ERROR: could not start server"
        return -1
    else
        echo "OK"
    fi

    # start client
    command="A 147.188.192.41 443"
    echo -en "executing client: \t"
    ./$client $IPADDRESS $PORT $command > $clientOut 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo -e "Error: Could not execute client"
        killall $server > /dev/null 2> /dev/null
        return -1
    else
        echo "OK"
    fi
    killall $server > /dev/null 2> /dev/null
    if [ ! -r $clientOut ]
    then
        echo "Error: Client produced no output"
        return -1
    fi
    
    echo -en "server result:     \t"
    res=`diff $clientOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
    return 0
}

function add_rule_testcase(){
    t="add rule test case"
    #cleanup
    rm -f $serverOut
    rm -f $clientOut
    rm -f $successFile
    echo "Rule added" > $successFile
    killall $server > /dev/null 2> /dev/null

    # start server
    echo -en "starting server: \t"
    ./$server $PORT > $serverOut  2>&1 &
    checkConnection
    if [ $? -ne 1 ]
    then
        echo -e "ERROR: could not start server"
        return -1
    else
        echo "OK"
    fi

    # start client
    command="A 192.168.1.1-192.168.1.10 300-400"
    echo -en "executing client: \t"
    ./$client $IPADDRESS $PORT $command > $clientOut 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo -e "Error: Could not execute client"
        killall $server > /dev/null 2> /dev/null
        return -1
    else
        echo "OK"
    fi
    killall $server > /dev/null 2> /dev/null
    if [ ! -r $clientOut ]
    then
        echo "Error: Client produced no output"
        return -1
    fi
    
    echo -en "server result:     \t"
    res=`diff $clientOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
    return 0
}

function query_rule_testcase(){
    t="query rule test case"
    #cleanup
    rm -f $serverOut
    rm -f $clientOut
    rm -f $successFile
    echo "Query: 192.168.1.2 350" > $successFile
    killall $server > /dev/null 2> /dev/null

    # start server
    echo -en "starting server: \t"
    ./$server $PORT > $serverOut  2>&1 &
    checkConnection
    if [ $? -ne 1 ]
    then
        echo -e "ERROR: could not start server"
        return -1
    else
        echo "OK"
    fi

    # start client
    command="Q 192.168.1.2 350"
    echo -en "executing client: \t"
    ./$client $IPADDRESS $PORT $command > $clientOut 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo -e "Error: Could not execute client"
        killall $server > /dev/null 2> /dev/null
        return -1
    else
        echo "OK"
    fi
    killall $server > /dev/null 2> /dev/null
    if [ ! -r $clientOut ]
    then
        echo "Error: Client produced no output"
        return -1
    fi
    
    echo -en "server result:     \t"
    res=`diff $clientOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
    return 0
}

function delete_rule_testcase(){
    t="delete rule test case"
    #cleanup
    rm -f $serverOut
    rm -f $clientOut
    rm -f $successFile
    echo "Rule invalid" > $successFile
    killall $server > /dev/null 2> /dev/null

    # start server
    echo -en "starting server: \t"
    ./$server $PORT > $serverOut  2>&1 &
    checkConnection
    if [ $? -ne 1 ]
    then
        echo -e "ERROR: could not start server"
        return -1
    else
        echo "OK"
    fi

    # start client
    command="D 192.168.1.1-192.168.1.10 300-400"
    echo -en "executing client: \t"
    ./$client $IPADDRESS $PORT $command > $clientOut 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo -e "Error: Could not execute client"
        killall $server > /dev/null 2> /dev/null
        return -1
    else
        echo "OK"
    fi
    killall $server > /dev/null 2> /dev/null
    if [ ! -r $clientOut ]
    then
        echo "Error: Client produced no output"
        return -1
    fi
    
    echo -en "server result:     \t"
    res=`diff $clientOut $successFile 2>&1`
    if [ " $res" != " " ]
    then
        echo "Error: Server returned invalid result"
        return -1
    else
        echo "OK"
    fi
    return 0
}

# --- execution ---

run interactive_testcase
run interactive_add_rule_testcase
run interactive_query_rule_testcase
run interactive_delete_rule_testcase

#cleanup
if [ $ret != 0 ]
then
    echo Basic test failed
else
    echo Basic test succeeded
fi
exit $ret