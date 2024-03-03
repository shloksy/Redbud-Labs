var srs =
[
    [ "Introduction", "srs.html#srs_intro", [
      [ "Audience", "srs.html#srs_audience", null ]
    ] ],
    [ "Document Conventions", "srs.html#srs_conv", [
      [ "Requirement UIDs", "srs.html#srs_uid", null ],
      [ "Use of Shall/Should/etc.", "srs.html#srs_lang", [
        [ "\"shall\"", "srs.html#srs_lang_shall", null ],
        [ "\"should\"", "srs.html#srs_lang_should", null ],
        [ "\"may\"", "srs.html#srs_lang_may", null ],
        [ "\"must not\"", "srs.html#srs_lang_must-not", null ]
      ] ]
    ] ],
    [ "SRS Organization", "srs.html#srs_org", [
      [ "References", "srs.html#srs_ref", null ]
    ] ],
    [ "Overview", "srs_over.html", [
      [ "Context and Functional Decomposition", "srs_over.html#srs_over_ctxt", [
        [ "Event-Driven Paradigm", "srs_over.html#srs_over_ed", null ],
        [ "Inversion of Control", "srs_over.html#srs_over_inv", null ],
        [ "Framework, NOT a Library", "srs_over.html#srs_over_frame", null ],
        [ "Active Objects", "srs_over.html#srs_intro_ao", null ],
        [ "State Machines", "srs_over.html#srs_over_sm", null ]
      ] ],
      [ "Portability & Configurability of QP", "srs_over.html#srs_conf", [
        [ "Compile-Time Configurability", "srs_over.html#srs_conf_compile", null ],
        [ "Run-Time Configurability", "srs_over.html#srs_conf_run", null ]
      ] ]
    ] ],
    [ "Events", "srs_evt.html", [
      [ "Event Signal", "srs_evt.html#srs_evt_sig", null ],
      [ "Event Parameters", "srs_evt.html#srs_evt_par", null ],
      [ "Requirements", "srs_evt.html#src_evt_req", [
        [ "RQP001", "srs_evt.html#RQP001", null ],
        [ "RQP002", "srs_evt.html#RQP002", null ],
        [ "RQP002A", "srs_evt.html#RQP002A", null ],
        [ "RQP002B", "srs_evt.html#RQP002B", null ],
        [ "RQP002C", "srs_evt.html#RQP002C", null ],
        [ "RQP003", "srs_evt.html#RQP003", null ],
        [ "RQP004", "srs_evt.html#RQP004", null ],
        [ "RQP005", "srs_evt.html#RQP005", null ],
        [ "RQP006", "srs_evt.html#RQP006", null ]
      ] ]
    ] ],
    [ "State Machines", "srs_sm.html", [
      [ "State Machine Implementation Strategies", "srs_sm.html#srs_sm_impl", null ],
      [ "Dispatching Events to a State Machine in QP Framework", "srs_sm.html#src_sm_proc", [
        [ "State Machine Specification", "srs_sm.html#srs_sm_spec", null ],
        [ "State Machine Processor", "srs_sm.html#srs_sm_proc", null ],
        [ "Run To Completion (RTC) Processing", "srs_sm.html#srs_sm_rtc", null ]
      ] ],
      [ "Requirements", "srs_sm.html#src_sm_req", [
        [ "RQP101", "srs_sm.html#RQP101", null ],
        [ "RQP102", "srs_sm.html#RQP102", null ],
        [ "RQP103", "srs_sm.html#RQP103", null ],
        [ "RQP104", "srs_sm.html#RQP104", null ],
        [ "RQP105", "srs_sm.html#RQP105", null ],
        [ "RQP110", "srs_sm.html#RQP110", null ],
        [ "RQP111", "srs_sm.html#RQP111", null ],
        [ "RQP120", "srs_sm.html#RQP120", null ],
        [ "RQP120A", "srs_sm.html#RQP120A", null ],
        [ "RQP120B", "srs_sm.html#RQP120B", null ],
        [ "RQP120C", "srs_sm.html#RQP120C", null ],
        [ "RQP120D", "srs_sm.html#RQP120D", null ],
        [ "RQP120E", "srs_sm.html#RQP120E", null ],
        [ "RQP120F", "srs_sm.html#RQP120F", null ],
        [ "RQP120G", "srs_sm.html#RQP120G", null ],
        [ "RQP120I", "srs_sm.html#RQP120I", null ],
        [ "RQP120H", "srs_sm.html#RQP120H", null ],
        [ "RQP120S", "srs_sm.html#RQP120S", null ],
        [ "RQP120T", "srs_sm.html#RQP120T", null ],
        [ "RQP130", "srs_sm.html#RQP130", null ],
        [ "RQP130A", "srs_sm.html#RQP130A", null ],
        [ "RQP130B", "srs_sm.html#RQP130B", null ],
        [ "RQP130C", "srs_sm.html#RQP130C", null ],
        [ "RQP130D", "srs_sm.html#RQP130D", null ],
        [ "RQP130E", "srs_sm.html#RQP130E", null ],
        [ "RQP130F", "srs_sm.html#RQP130F", null ]
      ] ]
    ] ],
    [ "Active Objects", "srs_ao.html", [
      [ "Active Objects in QP Framework", "srs_ao.html#src_ao_system", null ],
      [ "Encapsulation", "srs_ao.html#src_ao_enc", null ],
      [ "Asynchronous Communication", "srs_ao.html#src_ao_asynch", null ],
      [ "Run-to-Completion (RTC)", "srs_ao.html#src_ao_rtc", null ],
      [ "No Blocking", "srs_ao.html#src_ao_blocking", null ],
      [ "Thread of Control", "srs_ao.html#src_ao_thr", null ],
      [ "Active Object Priority", "srs_ao.html#src_ao_prio", null ],
      [ "Requirements", "srs_ao.html#src_ao_req", [
        [ "RQP200", "srs_ao.html#RQP200", null ]
      ] ]
    ] ],
    [ "Event Delivery", "srs_delivery.html", [
      [ "Direct Event Posting", "srs_delivery.html#src_ao_direct", null ],
      [ "Publish-Subscribe", "srs_delivery.html#src_ed_ps", null ],
      [ "Event Memory Management", "srs_delivery.html#src_ed_epool", [
        [ "Immutable Events", "srs_delivery.html#src_ed_immutable", null ],
        [ "Mutable Events", "srs_delivery.html#src_ed_mutable", null ]
      ] ],
      [ "Zero-Copy Event Delivery", "srs_delivery.html#src_ed_zero", null ],
      [ "Requirements", "srs_delivery.html#src_ed_req", [
        [ "RQP300", "srs_delivery.html#RQP300", null ]
      ] ]
    ] ],
    [ "Time Management", "srs_time.html", [
      [ "Time Events", "srs_time.html#srs_te", null ],
      [ "System Clock Tick", "srs_time.html#srs_tick", null ],
      [ "Requirements", "srs_time.html#src_te_req", [
        [ "RQP301", "srs_time.html#RQP301", null ]
      ] ]
    ] ],
    [ "Software Tracing", "srs_qs.html", [
      [ "Requirements", "srs_qs.html#srs_qs_req", [
        [ "RQP401", "srs_qs.html#RQP401", null ]
      ] ]
    ] ],
    [ "Cooperative Run-to-Completion Kernel", "srs_qv.html", [
      [ "Theory of Operation", "srs_qv.html#srs_qv_theory", null ],
      [ "Requirements", "srs_qv.html#src_qv_req", [
        [ "RQP501", "srs_qv.html#RQP501", null ]
      ] ]
    ] ],
    [ "Preemptive Run-to-Completion Kernel", "srs_qk.html", [
      [ "Theory of Operation", "srs_qk.html#srs_qk_theory", null ],
      [ "Requirements", "srs_qk.html#src_qk_req", [
        [ "RQA001", "srs_qk.html#RQA001", null ],
        [ "RQA002", "srs_qk.html#RQA002", null ]
      ] ]
    ] ],
    [ "Preemptive Dual-Mode Kernel", "srs_qxk.html", [
      [ "Theory of Operation", "srs_qxk.html#srs_qxk_theory", null ],
      [ "Basic Threads", "srs_qxk.html#qxk_basic", null ],
      [ "Extended Threads", "srs_qxk.html#qxk_extended", null ],
      [ "QXK Feature Summary", "srs_qxk.html#qxk_features", [
        [ "Thread Local Storage", "srs_qxk.html#qxk_tls", null ]
      ] ],
      [ "Requirements", "srs_qxk.html#srs_qxk_req", [
        [ "RQP701", "srs_qxk.html#RQP701", null ]
      ] ]
    ] ],
    [ "Quality Attributes", "srs_qa.html", [
      [ "Compliance with Standards", "srs_qa.html#srs_std", null ],
      [ "Software Quality Attributes", "srs_qa.html#srs_quality", null ],
      [ "Performance Requirements", "srs_qa.html#srs_perform", null ],
      [ "Portability Requirements", "srs_qa.html#srs_port", null ],
      [ "Ease of Development Requirements", "srs_qa.html#srs_develop", null ],
      [ "Safety Requirements", "srs_qa.html#srs_safe", null ],
      [ "Security Requirements", "srs_qa.html#srs_secure", [
        [ "RQP91", "srs_qa.html#RQP91", null ],
        [ "RQP920", "srs_qa.html#RQP920", null ],
        [ "RQP911", "srs_qa.html#RQP911", null ]
      ] ]
    ] ],
    [ "QP Application Requirements", "srs_app.html", [
      [ "Requirements", "srs_app.html#src_app_req", [
        [ "RQP801", "srs_app.html#RQP801", null ]
      ] ]
    ] ]
];