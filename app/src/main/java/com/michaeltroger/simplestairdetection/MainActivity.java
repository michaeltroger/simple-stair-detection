package com.michaeltroger.simplestairdetection;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.databinding.DataBindingUtil;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.view.WindowManager;

import com.michaeltroger.simplestairdetection.databinding.ActivityMainBinding;

import java.text.SimpleDateFormat;
import java.util.ArrayList;

import static com.michaeltroger.simplestairdetection.SensorService.INTENT_ACTION;
import static com.michaeltroger.simplestairdetection.SensorService.INTENT_MSG;
import static com.michaeltroger.simplestairdetection.SensorService.STAIRS_MSG;
import static com.michaeltroger.simplestairdetection.SensorService.WALKING_MSG;

public class MainActivity extends AppCompatActivity {
    private BroadcastReceiver mUpdateUIReciver;
    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        binding = DataBindingUtil.setContentView(this, R.layout.activity_main);

        final Intent intent = new Intent(getApplicationContext(), SensorService.class );
        startService(intent);

        final ArrayList<String> list = new ArrayList<>();

        binding.recyclerView.setLayoutManager(new LinearLayoutManager(this));
        final MyRecyclerViewAdapter adapter = new MyRecyclerViewAdapter(this, list);
        binding.recyclerView.setAdapter(adapter);

        final IntentFilter filter = new IntentFilter(INTENT_ACTION);
        mUpdateUIReciver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                final long date = System.currentTimeMillis();

                final SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                final String dateString = sdf.format(date);

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
        registerReceiver(mUpdateUIReciver, filter);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        final Intent intent = new Intent(getApplicationContext(), SensorService.class);
        stopService(intent);

        unregisterReceiver(mUpdateUIReciver);
    }
}
