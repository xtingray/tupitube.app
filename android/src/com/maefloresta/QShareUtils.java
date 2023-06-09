//=============================================================================
// Copyright (c) 2014 Nicolas Froment

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//=============================================================================

package com.maefloresta;

import org.qtproject.qt5.android.QtNative;
import android.app.Activity;

import java.lang.String;
import android.content.Intent;
import android.util.Log;
import android.net.Uri;
import java.io.File;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import android.os.Build.VERSION;
import java.io.IOException;

public class QShareUtils
{
    public static boolean DEBUG = false;
    public static String logPath;
    public static File logFile;

    protected QShareUtils() {
    }

    public static void setLog(String log) {
        DEBUG = true;
        logPath = log + "/java.log";

        logFile = new File(logPath);
        try {
            if (logFile.exists()) {
                if (!logFile.delete())
                    Log.e("JAVA", "Can't remove previous Java log file! -> " + logPath);
            }
            logFile.createNewFile();
        } catch (IOException e) {
            Log.e("JAVA", "Can't create Java log file! -> " + logPath);
            e.printStackTrace();
        }

        String msg = "QShareUtils() - Log Path -> " + logPath;
        Log.i("JAVA", msg);
        appendLog(msg);
    }

    public static void shareUrl(String text, String url) {
        if (QtNative.activity() == null) {
            if (DEBUG) {
                String TAG = "ANDROID-JAVA/shareUrl()";
                String msg = "Fatal Error: Activity is NULL";
                Log.e(TAG, msg);
                appendLog(msg);
            }
            return;
        }

        Intent sendIntent = new Intent();
        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_TEXT, text + " " + url);
        sendIntent.setType("text/plain");
        QtNative.activity().startActivity(sendIntent);
    }

    public static void shareMedia(String target) throws IOException {
        String TAG = "ANDROID-JAVA/shareMedia()";
        if (DEBUG) {
            String msg = "Tracing intent...";
            Log.i(TAG, msg);
            appendLog(msg);
            msg = "Sharing media file -> " + target;
            Log.i(TAG, msg);
            appendLog(msg);
        }

        if (QtNative.activity() == null) {
            if (DEBUG) {
                String msg = "Fatal Error: Activity is NULL";
                Log.e(TAG, msg);
                appendLog(msg);
            }
            return;
        }

        String version = VERSION.RELEASE;
        int index = version.indexOf('.');
        String majorStr = "";
        if (index > 0)
            majorStr = version.substring(0, index);
        else
            majorStr = version;
        int major = Integer.parseInt(majorStr);

        if (DEBUG) {
            String msg = "Android version -> " + version;
            Log.i(TAG, msg);
            appendLog(msg);
            msg = "Android major version -> " + major;
            Log.i(TAG, msg);
            appendLog(msg);
        }

        String title = "Sharing TupiTube Image";
        String mimeType = "image/png";
        if (target.endsWith(".mp4")) {
            title = "Sharing TupiTube Animation";
            mimeType = "video/mp4";
        } else if (target.endsWith(".tup")) {
            title = "Sharing TupiTube Source File";
            mimeType = "application/zip";
        }

        if (target.endsWith(".tup")) {
            if (!makeTupFile(target)) {
                if (DEBUG) {
                    String msg = "Error while creating TUP file! :(";
                    Log.e(TAG, msg);
                    appendLog(msg);
                }
                return;
            }
        }

        // if (DEBUG)
        //     Log.i(TAG, "Format Version -> 1 (android < 6)");

        File mediaFile = new File(target);
        if (mediaFile.exists()) {
            Uri fileUri = Uri.fromFile(mediaFile);
            if (DEBUG) {
                String msg = fileUri.toString();
                Log.i("Uri: ", msg);
                appendLog("Uri: " + msg);
            }
            if (!launchIntent(fileUri, mimeType, title)) {
                if (DEBUG) {
                    String msg = "Fatal Error: Can not launch intent!";
                    Log.e(TAG, msg);
                    appendLog(msg);
                }
                return;
            }
        } else {
            if (DEBUG) {
                String msg = "Fatal Error: Media file doesn't exist -> " + target;
                Log.e(TAG, msg);
                appendLog(msg);
            }
            return;
        }
    }

    public static boolean makeTupFile(String target) throws IOException {
        String TAG = "ANDROID-JAVA/makeTupFile()";
        File tupFile = new File(target);
        String source = tupFile.getParent();

        if (DEBUG) {
            String msg = "Tracing source -> " + target;
            Log.i(TAG, msg);
            appendLog(msg);
            msg = "Tracing target -> " + source;
            Log.i(TAG, msg);
            appendLog(msg);
        }

        try {
            FileOutputStream fileWriter = new FileOutputStream(target);
            ZipOutputStream zip = new ZipOutputStream(fileWriter);

            boolean result = addFileToTup(zip, "animation/project.tpp", source + "/animation/project.tpp");
            result = addFileToTup(zip, "animation/library.tpl", source + "/animation/library.tpl");
            result = addFileToTup(zip, "animation/scene0.tps", source + "/animation/scene0.tps");
            if (!result) {
                if (DEBUG) {
                    String msg = "Fatal Error: Can't create TUP source file elements! :(";
                    Log.e(TAG, msg);
                    appendLog(msg);
                }
                return false;
            }

            zip.flush();
            zip.close();

            // Removing source file folder
            File sourceDir = new File(source + "/animation");
            if (sourceDir.isDirectory()) {
                String[] children = sourceDir.list();
                for (int i = 0; i < children.length; i++) {
                   new File(sourceDir, children[i]).delete();
                }
            }
            sourceDir.delete();
        } catch (IOException e) {
                if (DEBUG) {
                    String msg = "Fatal Error: " + e.getMessage();
                    Log.e(TAG, msg);
                    appendLog(msg);
                }
                return false;
        }

        return true;
    }

    public static boolean addFileToTup(ZipOutputStream zip, String fileEntry, String filePath) throws IOException {
        String TAG = "ANDROID-JAVA/addFileToTup()";
        if (DEBUG) {
            String msg = "Processing file: " + filePath;
            Log.i(TAG, msg);
            appendLog(msg);
        }
        byte[] buf = new byte[1024];
        int len;

        try {
            FileInputStream in = new FileInputStream(filePath);
            zip.putNextEntry(new ZipEntry(fileEntry));
            while ((len = in.read(buf)) > 0) {
                zip.write(buf, 0, len);
            }
            in.close();
        } catch (IOException e) {
            if (DEBUG) {
                String msg = "Fatal Error: " + e.getMessage();
                Log.e(TAG, msg);
                appendLog(msg);
            }
            return false;
        }

        return true;
    }

    public static boolean launchIntent(Uri fileUri, String mimeType, String title) {
        String TAG = "ANDROID-JAVA/launchIntent()";
        if (DEBUG) {
            String msg = "Running intent...";
            Log.i(TAG, msg);
            appendLog(msg);
        }

        /*
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setDataAndType(fileUri, mimeType);
        QtNative.activity().startActivity(intent);
        */

        Intent sendIntent = new Intent();
        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        sendIntent.putExtra(Intent.EXTRA_STREAM, fileUri);
        sendIntent.setType(mimeType);

        Intent chooser = Intent.createChooser(sendIntent, title);

        if (sendIntent.resolveActivity(QtNative.activity().getPackageManager()) != null) {
            if (DEBUG) {
                String msg = "Creating social media chooser...";
                Log.i(TAG, msg);
                appendLog(msg);
            }
            // QtNative.activity().startActivity(Intent.createChooser(sendIntent, title));
            QtNative.activity().startActivity(chooser);
        } else {
            if (DEBUG) {
                String msg = "Fatal Error: Intent not resolved!";
                Log.e(TAG, msg);
                appendLog(msg);
            }
            return false;
        }

        return true;
    }

    /*
    public static void showMessageDialog(String title, String message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(QtNative.activity());

        builder.setTitle(title);
        builder.setMessage(message)
               .setCancelable(true)
               .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                          dialog.cancel();
                        }
                });

        AlertDialog alertDialog = builder.create();
        alertDialog.show();
    }
    */

    private static void appendLog(String text) {
      try {
           BufferedWriter bw = new BufferedWriter(new FileWriter(logFile, true));
           bw.write(text + "<br/>");
           bw.newLine();
           bw.close();
      } catch (IOException e) {
           e.printStackTrace();
      }
    }

    /*
    private static void appendLog(String text) {
        FileWriter fr = null;
        BufferedWriter br = null;
        PrintWriter pr = null;
        try {
            fr = new FileWriter(logFile, true);
            br = new BufferedWriter(fr);
            pr = new PrintWriter(br);
            pr.println(text + "\n");
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                pr.close();
                br.close();
                fr.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    */
}
