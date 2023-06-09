package com.maefloresta;

import android.Manifest;
import java.lang.Runnable;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import java.util.Locale;

// import android.util.Log;

// Method to request permissions for the read and write from / to an external storage
public class QShowPermissionRationale
{
    // Activity variable passed in from the Qt program
    private final Activity m_MainActivity;
    
    // Method to request permissions
    public QShowPermissionRationale(Activity MainActivity)
    {
        m_MainActivity = MainActivity;

        MainActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                 String lang = Locale.getDefault().getLanguage().toLowerCase();
                 // Log.d("ANDROID-JAVA/QShowPermissionRationale()", "LANG -> " + lang);

                 String title = "Permission Denied";
                 String desc = "Without the external storage read/write permission all data used by the application will be stored in folders inaccessible to you. " +
                     "This includes the animations and images that surely you wold like to share with your friends.\n\n" +
                     "To fix this problem, we recommend you to reinstall the app once again, but allowing the permission requested when you launch the app for the first time.";

                 if (lang.equals("es")) {
                     title = "Permiso Denegado";
                     desc = "Sin los permisos de lectura/escritura sobre el almacenamiento externo, toda la información creada y usada por TupiTube será almacenada " +
                       " en carpetas a las que no tienes acceso. Esto incluye las animaciones y las imágenes que seguramente te gustaria compartir con tus amigos.\n\n" +
                       "Para corregir este problema, te recomendamos que reinstales TupiTube y aceptes el permiso solicitado cuando abras la aplicación por primera vez.";
                 } else if (lang.equals("pt")) {
                     title = "Permissão Negada";
                     desc = "Sem a permissão de leitura / gravação de armazenamento externo, todos os dados usados pelo Tupitube serão armazenados em pastas " +
                       "inacessíveis a você. Isto inclui as animações e imagens que certamente você gostaria de compartilhar com seus amigos.\n\n" +
                       "Para corrigir esse problema, recomendamos que você reinstale o TupiTube novamente, mas permita a permissão solicitada ao iniciar o aplicativo " +
                       "pela primeira vez.";
                 }

                 AlertDialog.Builder builder = new AlertDialog.Builder(m_MainActivity);
                 builder.setMessage(desc).setTitle(title);

                 builder.setNegativeButton("OK", new DialogInterface.OnClickListener() {
                     public void onClick(DialogInterface dialog, int id) {
                         m_MainActivity.finish();
                     }
                 });

                 AlertDialog dialog = builder.create();
                 dialog.show();  
            }
        });              
    }
}
