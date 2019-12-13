package com.google.tfmonitor

import android.content.Context
import android.net.Uri
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.Navigation
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.google.tuningfork.AppKey
import com.google.tuningfork.Annotation

// TODO: Rename parameter arguments, choose names that match
// the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
private const val ARG_PARAM1 = "param1"
private const val ARG_PARAM2 = "param2"

class AnnotationsAdapter(private val myDataset: MonitorViewModel) : RecyclerView.Adapter<AnnotationsAdapter.ViewHolder>() {

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder.
    // Each data item is just a string in this case that is shown in a TextView.
    class ViewHolder(v: View) : RecyclerView.ViewHolder(v) {
        var paramNameTextView: TextView? = null
        var paramTypeTextView: TextView? = null
        var paramValueTextView: TextView? = null

        init {
            paramNameTextView = v.findViewById(R.id.paramNameTextView)
            paramTypeTextView = v.findViewById(R.id.paramTypeTextView)
            paramValueTextView = v.findViewById(R.id.paramValueTextView)
        }
    }

    // Create new views (invoked by the layout manager)
    override fun onCreateViewHolder(parent: ViewGroup,
                                    viewType: Int): AnnotationsAdapter.ViewHolder {
        // create a new view
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.fp_row_item, parent, false) as View
        return ViewHolder(view)
    }

    fun removePackage(s: String) : String {
        return s.split('.').last()
    }

    fun resolveEnum(type: String?, value: Int? ) : String {
        val desc = myDataset.getActiveAppData()?.value?.desc
        // NB: Value 0 => invalid, so show nothing
        if (desc==null || type==null || value==null || value==0) return ""
        val enum_value_list = desc.enums[type]
        if (enum_value_list==null) return ""
        if (value>=enum_value_list.count()) return ""
        return enum_value_list[value]
    }

    // Replace the contents of a view (invoked by the layout manager)
    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        val ann = getAnnotation(position)
        holder.paramNameTextView?.text = ann?.name ?: ""
        holder.paramTypeTextView?.text = if (ann?.type != null) removePackage(ann?.type) else  ""
        holder.paramValueTextView?.text = resolveEnum(ann?.type, ann?.value)
    }

    private fun numFPs() = (myDataset.getActiveAppData()?.value?.desc?.annotations?.size ?: 0)

    private fun getAnnotation(pos: Int) : Annotation? {
        return myDataset.getActiveAppData()?.value?.desc?.annotations?.get(pos)
    }
    // Return the size of your dataset (invoked by the layout manager)
    override fun getItemCount() = numFPs()

    companion object {
        private val TAG = "AnnotationsAdapter"
    }
}

/**
 * A simple [Fragment] subclass.
 * Activities that contain this fragment must implement the
 * [Annotations.OnFragmentInteractionListener] interface
 * to handle interaction events.
 * Use the [Annotations.newInstance] factory method to
 * create an instance of this fragment.
 */
class AnnotationsFragment : Fragment() {
    // TODO: Rename and change types of parameters
    private var param1: String? = null
    private var param2: String? = null
    private var listener: OnFragmentInteractionListener? = null

    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    private lateinit var model : MonitorViewModel

    private fun getViewModel() : MonitorViewModel {
        val factory = ViewModelProvider.AndroidViewModelFactory.getInstance(activity?.application!!)
        return ViewModelProvider(activity?.viewModelStore!!, factory).get(MonitorViewModel::class.java)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        arguments?.let {
            param1 = it.getString(ARG_PARAM1)
            param2 = it.getString(ARG_PARAM2)
        }
        model = getViewModel()
        model.getLiveApps().observe(this, Observer<List<AppKey>> {
            viewAdapter.notifyDataSetChanged()
        })
        model.getStoredApps().observe(this, Observer<List<AppKey>> {
            viewAdapter.notifyDataSetChanged()
        })
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        val view = inflater.inflate(R.layout.fragment_annotations, container, false)

        viewManager = LinearLayoutManager(view.context)
        viewAdapter = AnnotationsAdapter(model)

        if (view!=null) {
            recyclerView = view.findViewById<RecyclerView>(R.id.annotations_recycler_view).apply {
                layoutManager = viewManager
                adapter = viewAdapter
            }
        }
        return view
    }

    // TODO: Rename method, update argument and hook method into UI event
    fun onButtonPressed(uri: Uri) {
        listener?.onFragmentInteraction(uri)
    }

    override fun onAttach(context: Context) {
        super.onAttach(context)
        if (context is OnFragmentInteractionListener) {
            listener = context
        } else {
            throw RuntimeException(context.toString() + " must implement OnFragmentInteractionListener")
        }
    }

    override fun onDetach() {
        super.onDetach()
        listener = null
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     *
     *
     * See the Android Training lesson [Communicating with Other Fragments]
     * (http://developer.android.com/training/basics/fragments/communicating.html)
     * for more information.
     */
    interface OnFragmentInteractionListener {
        // TODO: Update argument type and name
        fun onFragmentInteraction(uri: Uri)
    }

    companion object {
        /**
         * Use this factory method to create a new instance of
         * this fragment using the provided parameters.
         *
         * @param param1 Parameter 1.
         * @param param2 Parameter 2.
         * @return A new instance of fragment Annotations.
         */
        // TODO: Rename and change types and number of parameters
        @JvmStatic
        fun newInstance(param1: String, param2: String) =
            AnnotationsFragment().apply {
                arguments = Bundle().apply {
                    putString(ARG_PARAM1, param1)
                    putString(ARG_PARAM2, param2)
                }
            }
    }
}
