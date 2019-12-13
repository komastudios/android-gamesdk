package com.google.tuningfork

data class AppKey(public val name: String, public val version: Int) {

    companion object {
        fun parse(path_in: String): AppKey {
            // Paths are like: [/]applications/com.tuningfork.demoapp/apks/16
            // Ignore any initial /
            val path = {
                if (path_in.startsWith('/')) {
                    path_in.substring(1)
                } else {
                    path_in
                }
            }()
            val path_parts = path.split("/".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
            if (path_parts.size != 4 || path_parts[0] != "applications" || path_parts[2] != "apks") {
                throw RuntimeException("Unexpected part to path$path")
            }
            val app_name = path_parts[1]
            val app_version = Integer.valueOf(path_parts[3])
            return AppKey(app_name, app_version)
        }
    }
}

