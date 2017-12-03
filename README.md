# appctl

## Usage

For information about how to use appctl run it without options or with --help option:
```
$ ./bin/appctl --help
appctl version 0.0.1

usage:
    appctl [options] action { all | app1 app2 ... }
    appctl { --local | -l } [options] action application [application's options]

    options:
           --help               print this help and exit;
           --version            print script's version and exit;
        -l|--local              run at localhost (only one application with options);
        -f|--force              force action;
        -h|--hosts {h1,h2,...}  run commands on dedicated hosts (application should be configured to run on them).

        caution: run with local option doesn't check 'app.hosts', combine local and non-local runs carefully.
    actions (required):
        sync    sync binary and configuration files;
        start   start application;
        stop    stop application;
        restart restart application;
        status  check application's status.

    "all" keyword for applications means 'all configured applications at all configured hosts'.
```

## Installation

 1. Copy appctl script to any suitable place, for example to `$HOME/bin` directory.
 2. Adjust `APP_CONF_DIR` (place where appctl's configuration files will be placed), by default appctl is searching for configuration in place from which it is launched.
 3. If required, tune other environment variables, see section below for detailed description.
 4. Create and adjust configuration files app.conf and app.hosts.
 5. Place applications' scripts into directory to which points `APP_BIN_DIR`.
 6. Check all hosts configured in app.hosts are available via ssh. For example you can use this script:
```
for host in $(sed 's/#.*//' ${APP_CONF_DIR}/app.hosts | awk '{print $1}'); do
  ssh $host date 2>&1 | sed "s/^/$host: /"
done
```
 7. Run "appctl sync" for sync all configuration and binary. If something goes wrong – check log files.
 8. Run "appctl status all" to check appctl environment is configured and ready to use.

## Configuration

### environment variables
Variable       | Description  | Values  | Default value |  Comment 
---------------|--------------|---------|---------------|----------
APP_CONF_DIR   | Directory where configuration files are located   | | script location |
APP_BIN_DIR    | Directory where applications' scripts are located | | script location |
APP_LOG_DIR    | Log file location | | /tmp | appctl-{hostname}.log
APP_PID_DIR    | Directory for pid and lock files   | | /tmp | |
APP_LOG_LEVEL    | appctl log level | debug, info, warning, error, critical | info | scripts' output logged with 'debug' level
APP_SYNC_PATH  | List (space separated) of files and directories for synchronization via rsync/ssh | APP_{CONF,BIN}_DIR |

### app.conf

Configuration determining how to control applications.

Format: `<application name> [option[=value],option[=value],...]`

Option          | Description | Values | Default value
----------------|-------------|--------|---------------
start           | Starting script | application's name
stop            | Stopping script | | --
status          | Status script   | | --
run             | "Run" script. Just synonym for "start=script,stop=script,status=script" | | --
control         | How to control application's behavior | disable: don't control application, just run configured scripts; export: provide exports for application's scripts; full: full control (demonize application and control pid file) | disable
start_delay     | Delay in seconds after application start and before status checking | | 1
stop_delay      | Delay in seconds after application stop and before status checking  | | 1

Sample:
```
hdfs            start=start-dfs.sh,stop=stop-dfs.sh,status=hdfs-status
hbase           start=start-hbase.sh,stop=stop-hbase.sh,status=hbase-status
mysql           run=mysql
elasticsearch   control=export
backend         control=export,start=backend-runner,stop_delay=5
frontend        control=full
```

### app.hosts

Configuration of hosts where applications can be running. If application is not presented here it's considered as "disallowed".

Format: `<hostname> <application name>[,<application name>][,...]`

Sample:
```
dev-stable-master      hdfs,hbase,mysql,haproxy,frontend
dev-stable-slave-01    elasticsearch,backend
dev-stable-slave-02    elasticsearch,backend
dev-stable-slave-03    elasticsearch,backend
```

### app.env

Any environments variable could be placed here and, if exported, will be available for all applications. This file is sourced by appctl at the start.

app.env is good place for common variables like `JAVA_HOME` or `PYTHONPATH` also appctl related variabled (`APP_BIN_DIR`, `APP_LOG_DIR`, etc) can be located here.

You **can not** put `APP_CONF_DIR` here because appctl is using `APP_CONF_DIR` for searching location of app.env. But you can source app.env on shell startup (in .bashrc for example) and, in this case, `APP_CONF_DIR` could be defined:
```
# app.env
export APP_CONF_DIR=$HOME/etc
export APP_BIN_DIR=$HOME/opt/appctl

APP_LOG_DIR=$HOME/var/log
APP_RUN_DIR=$HOME/var/run

APP_LOG_LEVEL='debug'
APP_SYNC_PATH="$HOME/opt $HOME/.profile $HOME/.bashrc"

# Common environment
export JAVA_HOME=$HOME/opt/jdk
export TOMCAT_HOME=$HOME/opt/tomcat
...
```

If you need to place applications specific variables you can put them into file "$APP_CONF_DIR/<application name>.env" (appctl load it before perform any application's action).

## API (integration)
Environment variables `APP_X_NAME` (application's name) and `APP_X_PID_FILE` (full path to application's pid file) are exported for applications with control options is equal to "export". Application script can check for `APP_X_NAME` existence and should write your pid file into `APP_X_PID_FILE`.
