package com.google.tfmonitor

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider

class LogFragment : Fragment() {

    private lateinit var model : MonitorViewModel

    private var logTextView: TextView? = null

    private fun getViewModel() : MonitorViewModel {
        val factory = ViewModelProvider.AndroidViewModelFactory.getInstance(activity?.application!!)
        return ViewModelProvider(activity?.viewModelStore!!, factory).get(MonitorViewModel::class.java)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        model = getViewModel()
        model.getLog().observe(this, Observer<String> { logStr ->
            logTextView?.text = logStr
        })
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_log, container, false)
        logTextView = view?.findViewById(R.id.log_text_view)
        logTextView?.text = model.getLog().value
        return view
    }
}
