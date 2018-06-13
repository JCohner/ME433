package com.example.hw17_app;

// libraries

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.SeekBar;

import java.io.IOException;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;
import static android.icu.lang.UCharacter.GraphemeClusterBreak.T;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;

    SeekBar redThreshControl;
    SeekBar blueThreshControl;
    SeekBar greenThreshControl;
    TextView redThreshTextView;
    TextView blueThreshTextView;
    TextView greenThreshTextView;
    static long prevtime = 0; // for FPS calculation


    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mTextView = (TextView) findViewById(R.id.cameraStatus);
        redThreshControl = (SeekBar) findViewById(R.id.seekRed);
        blueThreshControl = (SeekBar) findViewById(R.id.seekBlue);
        greenThreshControl = (SeekBar) findViewById(R.id.seekGreen);

        redThreshTextView = (TextView) findViewById(R.id.threshRed);
        redThreshTextView.setText("Set initial value");
        blueThreshTextView = (TextView) findViewById(R.id.threshRed);
        blueThreshTextView.setText("Set initial value");
        greenThreshTextView = (TextView) findViewById(R.id.threshRed);
        greenThreshTextView.setText("Set initial value");

        setRedControlListener();
        setBlueControlListener();
        setGreenControlListener();

        // see if the app has permission to use the camera
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        //parameters.setAutoExposureLock(true); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {
            int threshRed = getRedThresh();//0; // comparison value
            int threshBlue = getBlueThresh();
            int threshGreen = getGreenThresh();
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data
            //int startY = 200; // which row in the bitmap to analyze to read


            /*
            for (int j = 0; j < bmp.getHeight(); j++) {
                bmp.getPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);
                // in the row, see if there is more green than red
                for (int i = 0; i < bmp.getWidth(); i++) {
                    if (((green(pixels[i]) - red(pixels[i])) >thresh) && ((green(pixels[i]) - blue(pixels[i])) > thresh)) {
                        pixels[i] = rgb(0, 255, 0); // over write the pixel with pure green
                        // update the row
                        bmp.setPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);
                    }
                }
            }
            */
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, 200, bmp.getWidth(), 1);
            int sum_mr = 0; // the sum of the mass times the radius
            int sum_m = 0; // the sum of the masses
            for (int i = 0; i < bmp.getWidth(); i++) {
                if ( ((red(pixels[i]) - (green(pixels[i]) + blue(pixels[i]))/2) > -threshRed) && ((red(pixels[i]) - (green(pixels[i])+blue(pixels[i]))/2) < threshRed) && (red(pixels[i])  > threshGreen) ) {
                    pixels[i] = rgb(1, 1, 1); // set the pixel to almost 100% black

                    sum_m = sum_m + green(pixels[i])+red(pixels[i])+blue(pixels[i]);
                    sum_mr = sum_mr + (green(pixels[i])+red(pixels[i])+blue(pixels[i]))*i;
                }
            }
            // only use the data if there were a few pixels identified, otherwise you might get a divide by 0 error
            if(sum_m>5){
                //COM = sum_mr / sum_m;
            }
            else{
                //COM = 0;
            }

        }

        // draw a circle at some position
        int pos = 50;
        canvas.drawCircle(pos, 240, 5, paint1); // x position, y position, diameter, color

        // write the pos as text
        canvas.drawText("pos = " + pos, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("FPS " + 1000 / diff);
        prevtime = nowtime;
    }
    public int getRedThresh() {
        SeekBar seekBar = findViewById(R.id.seekRed);
        int value = seekBar.getProgress();
        return value;
    }
    public int getBlueThresh() {
        SeekBar seekBar = findViewById(R.id.seekBlue);
        int value = seekBar.getProgress();
        return value;
    }
    public int getGreenThresh() {
        SeekBar seekBar = findViewById(R.id.seekGreen);
        int value = seekBar.getProgress();
        return value;
    }

    private void setRedControlListener() {
        redThreshControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                redThreshTextView.setText("Red thresh is: "+ progress);

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
    private void setBlueControlListener() {
        blueThreshControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                blueThreshTextView.setText("Blue thresh is: "+ progress);

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
    private void setGreenControlListener() {
        greenThreshControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                greenThreshTextView.setText("Green thresh is: "+ progress);

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
}