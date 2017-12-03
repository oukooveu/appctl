# common configuration and checks for init scripts

runpath=$(cd $(dirname $(readlink -f $0)) && pwd)
script=$(basename $0)
host=$(hostname)

function out_log {
    severity="$1" && shift
    message="$@"
    timestamp=$(date +'%Y-%m-%d %H:%M:%S')
    echo -e "${timestamp} : ${host} : ${script} : ${message}"
}

function tee_redirect {
    log_file=$1
    exec 3>&1
    exec 4>&2
    exec &> >(tee ${log_file}) 2>&1
}

function restore_redirect {
    exec >&3
    exec 2>&4
}

APP_CONF_DIR=${APP_CONF_DIR:-$(cd ${runpath}/../../conf && pwd)}
APP_LOG_DIR=${APP_LOG_DIR:-$(cd ${runpath}/../../logs && pwd)}

[ -d ${APP_LOG_DIR} ] || mkdir -p ${APP_LOG_DIR}

# EOF
