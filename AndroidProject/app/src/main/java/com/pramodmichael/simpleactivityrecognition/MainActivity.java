package com.pramodmichael.simpleactivityrecognition;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.WindowManager;

import java.text.SimpleDateFormat;
import java.util.ArrayList;

import static com.pramodmichael.simpleactivityrecognition.SensorService.INTENT_ACTION;
import static com.pramodmichael.simpleactivityrecognition.SensorService.INTENT_MSG;
import static com.pramodmichael.simpleactivityrecognition.SensorService.STAIRS_MSG;
import static com.pramodmichael.simpleactivityrecognition.SensorService.WALKING_MSG;

public class MainActivity extends AppCompatActivity {
    private BroadcastReceiver mUpdateUIReciver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        Intent intent = new Intent(getApplicationContext(), SensorService.class );
        startService(intent);

        final ArrayList<String> list = new ArrayList<>();

        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.recyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        final MyRecyclerViewAdapter adapter = new MyRecyclerViewAdapter(this, list);
        recyclerView.setAdapter(adapter);

        IntentFilter filter = new IntentFilter(INTENT_ACTION);
        mUpdateUIReciver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                long date = System.currentTimeMillis();

                SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                String dateString = sdf.format(date);

                switch (intent.getStringExtra(INTENT_MSG)) {
                    case WALKING_MSG:
                        list.add(dateString + ": " + getString(R.string.walking_detected));
                        break;
                    case STAIRS_MSG:
                        list.add(dateString + ": " + getString(R.string.stairs_detected));
                        break;
                }

                adapter.notifyDataSetChanged();
            }
        };
        registerReceiver(mUpdateUIReciver,filter);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        Intent intent = new Intent(getApplicationContext(), SensorService.class );
        stopService(intent);

        unregisterReceiver(mUpdateUIReciver);
    }
}
