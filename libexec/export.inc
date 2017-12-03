# libexec/export.inc
#
# include file for start scripts with 'export' control option
#

[ "x${APP_X_NAME}" == "x" ] && { echo "$(basename $0): script should be running via appctl"; exit 1; }

# bash variables can't contain hyphens, all hyphens are transformed to underscores
app_name=${APP_X_NAME//\-/\_}

# variable stores application's home variable name
home_dir="${app_name^^}_HOME"

# if application's home is not defined we can try to calculate it in case when installation was performed via appinstall
if [ "x${!home_dir}" == "x" ]; then
	if [ "x${APP_INSTALL_HOME}" != "x" ]; then
		eval ${home_dir}="${APP_INSTALL_HOME}/${APP_X_NAME}/${APP_INSTALL_LINK}"
	fi
fi

# for all scripts application's home must exist because we don't know how application was installed
if [ ! -d "${!home_dir}" ]; then
	echo "${home_dir} (${!home_dir}) should be defined and exists. Exiting."
	exit 1
fi

# EOF