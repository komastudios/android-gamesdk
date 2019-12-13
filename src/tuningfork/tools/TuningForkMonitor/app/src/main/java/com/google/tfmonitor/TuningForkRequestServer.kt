/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.tfmonitor

import android.net.Uri
import com.google.tfmonitor.RequestListener.debugInfo
import com.google.tfmonitor.RequestListener.generateTuningParameters
import com.google.tfmonitor.RequestListener.uploadTelemetry
import com.google.tuningfork.AppKey
import java.io.DataInputStream
import java.io.DataOutputStream
import java.io.IOException
import java.io.PrintWriter
import java.io.BufferedWriter
import java.io.OutputStreamWriter
import java.net.InetAddress
import java.net.ServerSocket
import java.net.Socket
import java.net.SocketException
import java.net.SocketTimeoutException
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.TimeZone

fun String.HeaderTrim() =
    this.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()[1].trim { it <= ' ' }

class TuningForkRequestServer(ip: String, port: Int) : Thread() {
    private val serverSocket: ServerSocket
    private var isStart = true
    private var keepAlive = true
    private var serverName = "TuningForkRequestServer v1.0"

    init {
        val addr = InetAddress.getByName(ip)
        serverSocket = ServerSocket(port, 100, addr)
        serverSocket.setSoTimeout(5000)  //set timeout for listener
    }

    override fun run() {
        while (isStart) {
            try {
                //wait for new connection on port
                val newSocket = serverSocket.accept()
                val newClient = ResponseThread(newSocket)
                newClient.start()
            } catch (s: SocketTimeoutException) {
            } catch (e: IOException) {
            }

        }
    }

    inner class ResponseThread(protected var socket: Socket) : Thread() {

        internal val TAG = "AppApis"

        private var requestType = "GET"
        private var httpVer = "HTTP/1.1"
        private var contentType = ""
        private var status = "200"

        val kPageNotFound = "404"
        val kOkay = "200"

        val kContentTypePattern =
            Regex("([ |\t]*content-type[ |\t]*:)(.*)", RegexOption.IGNORE_CASE)
        val kContentLengthPattern = Regex("Content-Length:", RegexOption.IGNORE_CASE)
        val kUserAgentPattern = Regex("User-Agent:", RegexOption.IGNORE_CASE)
        val kClientHostPattern = Regex("Host:", RegexOption.IGNORE_CASE)
        val kConnectionTypePattern = Regex("Connection:", RegexOption.IGNORE_CASE)
        val kAcceptEncodingPattern = Regex("Accept-Encoding:", RegexOption.IGNORE_CASE)

        override fun run() {

            try {
                var inp: DataInputStream? = null
                var out: DataOutputStream? = null

                if (socket.isConnected) {
                    inp = DataInputStream(socket.getInputStream())
                    out = DataOutputStream(socket.getOutputStream())
                }

                val data = ByteArray(1500)
                socket.setSoTimeout(60 * 1000 * 5);

                while (true) {
                    val read_size = inp!!.read(data)
                    if (read_size == -1) break
                    var recData = String(data).trim { it <= ' ' }
                    val header = recData.split("\\r?\\n".toRegex()).dropLastWhile { it.isEmpty() }
                        .toTypedArray()

                    var contentLen = "0"
                    var contentType = "text/html"
                    var connectionType = "keep-alive"
                    var hostname = ""
                    var userAgent = ""
                    var encoding = ""

                    val h1 =
                        header[0].split(" ".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
                    if (h1.size == 3) {
                        requestType = h1[0]
                        httpVer = h1[2]
                    }

                    for (h in header.indices) {
                        val value = header[h].trim { it <= ' ' }

                        when (true) {
                            kContentLengthPattern.containsMatchIn(value) -> contentLen =
                                value.HeaderTrim()
                            kContentTypePattern.containsMatchIn(value) -> contentType =
                                value.HeaderTrim()
                            kConnectionTypePattern.containsMatchIn(value) -> connectionType =
                                value.HeaderTrim()
                            kClientHostPattern.containsMatchIn(value) -> hostname =
                                value.HeaderTrim()
                            kAcceptEncodingPattern.containsMatchIn(value) -> encoding =
                                value.HeaderTrim()
                            kUserAgentPattern.containsMatchIn(value) ->
                                for (ua in value.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()) {
                                    if (!ua.equals("User-Agent:", ignoreCase = true)) {
                                        userAgent += ua.trim { it <= ' ' }
                                    }
                                }
                        }
                    }

                    val contentLength = Integer.valueOf(contentLen)

                    if (requestType != "") {
                        var postData = ""
                        if (requestType.equals("POST", ignoreCase = true) && contentLen != "0") {
                            postData = header[header.size - 1]
                            if (contentLength > 0) {
                                // Read any extra data
                                while (contentLength > postData.length) {
                                    val read_size = inp!!.read(data)
                                    if (read_size <= 0) break
                                    val str = StringBuilder()
                                    recData = String(data).trim { it <= ' ' }
                                    str.append(postData)
                                    str.append(recData)
                                    postData = str.toString()
                                }
                                postData = postData.substring(0, contentLength)
                            }
                        }

                        val requestLocation = h1[1]
                        if (requestLocation != null) {
                            processLocation(out, requestLocation, postData)
                        }
                    }
                }
            } catch (er: SocketException) {

            } catch (er: Exception) {
                er.printStackTrace()
            }
        }

        fun processLocation(out: DataOutputStream?, location: String, postData: String) {
            var data = ""

            println("url location -> $location")
            val geturi = Uri.parse("http://localhost$location")
            val dirPath =
                geturi.path?.split("/".toRegex())?.dropLastWhile({ it.isEmpty() })?.toTypedArray()
                    ?: arrayOf()
            if (dirPath.size > 1) {
                val fileName = dirPath[dirPath.size - 1]
                var qparms = mutableMapOf<String, String?>()
                if (requestType == "POST") {
                    qparms.set("_POST", postData)
                }
                data = getResultByName(
                    dirPath.dropLast(1).joinToString("/"),
                    fileName,
                    qparms
                )
                if (out != null)
                    constructHeader(out, data.length.toString() + "", data)
            }
        }

        fun getResultByName(
            path: String,
            name: String,
            qparms: MutableMap<String, String?>
        ): String {
            var path = path
            val name_parts = name.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
            var function = name
            if (name_parts.size > 1) {
                function = name_parts[name_parts.size - 1]
                path += "/" + name_parts.dropLast(1).joinToString(":")
                qparms.set("PATH", path)
            }
            try {
                val app = AppKey.parse(path)
                val postData = qparms["_POST"]
                val resultBody = if (postData != null) {
                    when (function) {
                        "generateTuningParameters" -> generateTuningParameters(app, postData)
                        "uploadTelemetry" -> uploadTelemetry(app, postData)
                        "debugInfo" -> debugInfo(app, postData)
                        else -> errorResult()
                    }
                } else errorResult()
                status = kOkay
                return resultBody
            } catch (er: Exception) {
                er.printStackTrace()
                return errorResult()
            }
        }

        fun errorResult(): String {
            status = kPageNotFound
            return ("<!DOCTYPE html>"
                    + "<html><head><title>Page not found</title>"
                    + "</head><body><h3>Requested page not found</h3></body></html>")
        }

        private fun constructHeader(output: DataOutputStream, size: String, data: String) {
            val gmtFrmt = SimpleDateFormat("E, d MMM yyyy HH:mm:ss 'GMT'", Locale.US)
            gmtFrmt.timeZone = TimeZone.getTimeZone("GMT")
            val pw = PrintWriter(BufferedWriter(OutputStreamWriter(output)), false)
            pw.append("HTTP/1.1 ").append(status).append(" \r\n")
            if (contentType != null) {
                printHeader(pw, "Content-Type", contentType)
            }
            printHeader(pw, "Date", gmtFrmt.format(Date()))
            printHeader(pw, "Connection", if (keepAlive) "keep-alive" else "close")
            printHeader(pw, "Content-Length", size)
            printHeader(pw, "Server", serverName)
            pw.append("\r\n")
            pw.append(data)
            pw.flush()
            pw.close();
        }

        private fun printHeader(pw: PrintWriter, key: String, value: String) {
            pw.append(key).append(": ").append(value).append("\r\n")
        }

    }

    fun close() {
        if (isStart) {
            try {
                isStart = false
                println("Server stopped running !")
            } catch (er: IOException) {
                er.printStackTrace()
            }

        }
    }

    companion object {

        var isStart = false
        private var server: TuningForkRequestServer? = null

        fun startServer(ip: String, port: Int) {
            try {

                if (!isStart) {
                    server = TuningForkRequestServer(ip, port)
                    val eh =
                        UncaughtExceptionHandler { server, e -> println("Uncaught exception!") }
                    server?.uncaughtExceptionHandler = eh
                    server?.start()
                    println("Server Started on port $port!")
                    isStart = true
                }
            } catch (e: IOException) {
                e.printStackTrace()
            } catch (e: Exception) {
                e.printStackTrace()
            }

        }

        fun stopServer() {
            server?.close()
        }

    }

}