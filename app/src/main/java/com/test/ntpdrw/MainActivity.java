package com.test.ntpdrw;

import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.AbsListView;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.SeekBar;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /* 視点距離 */
        EditText ettvierpoint = (EditText)findViewById(R.id.ettviewpoint);
        int viewpoint = Integer.parseInt(ettvierpoint.getText().toString());
        NativeFuncforGes.setViewPoint(viewpoint);
        ettvierpoint.addTextChangedListener(new TextWatcher() {
            @Override public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) { }
            @Override public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) { }
            @Override
            public void afterTextChanged(Editable editable) {
                int viewpoint = Integer.parseInt(((EditText)findViewById(R.id.ettviewpoint)).getText().toString());
                NativeFuncforGes.setViewPoint(viewpoint);
            }
        });

        /* 回転シークバー */
        SeekBar seekrotate = (SeekBar)findViewById(R.id.seekrotate);
        ((TextView)findViewById(R.id.texrotate)).setText("" + (seekrotate.getProgress()-360));
        seekrotate.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean b) {
                ((TextView)findViewById(R.id.texrotate)).setText("" + (progress-360));
                NativeFuncforGes.setRotate(progress-360);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override public void onStopTrackingTouch(SeekBar seekBar) { }
        });

        /* 俯角シークバー */
        SeekBar seekdepress = (SeekBar)findViewById(R.id.seekdepress);
        seekdepress.setTranslationX(300);
        seekdepress.setRotation(270);
        ((TextView)findViewById(R.id.texdepress)).setText("" + (seekdepress.getProgress()-90));
        seekdepress.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean b) {
                ((TextView)findViewById(R.id.texdepress)).setText("" + (progress-90));
                NativeFuncforGes.setDepress(progress-90);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override public void onStopTrackingTouch(SeekBar seekBar) { }
        });

        SurfaceView surfacefull = ((SurfaceView)findViewById(R.id.surfacefull));
        /* 透過設定 */
        surfacefull.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        surfacefull.setZOrderOnTop(true);
        /* コールバック設定 */
        surfacefull.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                /* C++ */
                NativeFunc2.create(0);
                NativeFunc2.surfaceCreated(0, holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                NativeFunc2.surfaceChanged(0, width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                NativeFunc2.surfaceDestroyed(0);
            }
        });

        SurfaceView surface = ((SurfaceView)findViewById(R.id.surface));
        /* 透過設定 */
        surface.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        surface.setZOrderOnTop(true);
        /* コールバック設定 */
        surface.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                /* C++ */
//                NativeFunc.create(0);
//                NativeFunc.surfaceCreated(0, holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//                NativeFunc.surfaceChanged(0, width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
//                NativeFunc.surfaceDestroyed(0);
            }
        });
    }
}
