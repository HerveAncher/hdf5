SET (CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS 1500)
 
SET (CTEST_CUSTOM_WARNING_EXCEPTION
    ${CTEST_CUSTOM_WARNING_EXCEPTION}
    "H5detect.c.[0-9]+.[ \t]*:[ \t]*warning C4090"
    "testhdf5.h.[0-9]+.[ \t]*:[ \t]*warning C4005"
    "H5Ztrans.c.[0-9]+.[ \t]*:[ \t]*warning C4244"
    "SZIP.src.[0-9a-zA-Z]*warning"
    "POSIX name for this item is deprecated"
    "disabling jobserver mode"
)
 
SET (CTEST_CUSTOM_MEMCHECK_IGNORE
    ${CTEST_CUSTOM_MEMCHECK_IGNORE}
    h5test-clear-objects
    h5perform-clear-objects
    hl_test-clear-objects
    hl_fortran_test-clear-objects
    H5DIFF-clearall-objects
    H5LS-clearall-objects
    h5repart_20K-clear-objects
    h5repart_5K-clear-objects
    h5repart_sec2-clear-objects
    H5IMPORT-clear-objects
    H5REPACK-clearall-objects
    H5COPY-clearall-objects
    H5STAT-clearall-objects
    H5DUMP-clearall-objects
    H5DUMP-clear-out1
    H5DUMP-clear-out3
    H5DUMP-clear-objects
    H5DUMP_PACKED_BITS-clearall-objects
    H5DUMP-XML-clearall-objects
    #H5DIFF-h5diff_10
    H5DIFF-h5diff_11
    H5DIFF-h5diff_12
    H5DIFF-h5diff_13
    H5DIFF-h5diff_14
    #H5DIFF-h5diff_15
    #H5DIFF-h5diff_16_1
    H5DIFF-h5diff_16_2
    H5DIFF-h5diff_16_3
    H5DIFF-h5diff_17
    H5DIFF-h5diff_171
    H5DIFF-h5diff_172
    H5DIFF-h5diff_18
    H5DIFF-h5diff_18_1
    H5DIFF-h5diff_20
    H5DIFF-h5diff_21
    H5DIFF-h5diff_22
    H5DIFF-h5diff_23
    H5DIFF-h5diff_24
    H5DIFF-h5diff_25
    H5DIFF-h5diff_26
    H5DIFF-h5diff_27
    H5DIFF-h5diff_28
    H5DIFF-h5diff_50
    H5DIFF-h5diff_51
    H5DIFF-h5diff_52
    H5DIFF-h5diff_53
    H5DIFF-h5diff_54
    H5DIFF-h5diff_55
    H5DIFF-h5diff_56
    H5DIFF-h5diff_57
    H5DIFF-h5diff_58
    H5DIFF-h5diff_600
    H5DIFF-h5diff_601
    H5DIFF-h5diff_603
    H5DIFF-h5diff_604
    H5DIFF-h5diff_605
    H5DIFF-h5diff_606
    H5DIFF-h5diff_607
    H5DIFF-h5diff_608
    H5DIFF-h5diff_609
    H5DIFF-h5diff_610
    H5DIFF-h5diff_612
    H5DIFF-h5diff_613
    H5DIFF-h5diff_614
    H5DIFF-h5diff_615
    H5DIFF-h5diff_616
    H5DIFF-h5diff_617
    H5DIFF-h5diff_618
    H5DIFF-h5diff_619
    H5DIFF-h5diff_621
    H5DIFF-h5diff_622
    H5DIFF-h5diff_623
    H5DIFF-h5diff_624
    H5DIFF-h5diff_625
    H5DIFF-h5diff_626
    H5DIFF-h5diff_627
    H5DIFF-h5diff_628
    H5DIFF-h5diff_70
    H5DIFF-h5diff_80
    H5DIFF-h5diff_90
    H5DIFF-h5diff_101
    H5DIFF-h5diff_102
    H5DIFF-h5diff_200
    H5DIFF-h5diff_201
    H5DIFF-h5diff_202
    H5DIFF-h5diff_203
    H5DIFF-h5diff_204
    H5DIFF-h5diff_205
    H5DIFF-h5diff_206
    H5DIFF-h5diff_207
    H5DIFF-h5diff_300
    H5DIFF-h5diff_400
    H5DIFF-h5diff_401
    H5DIFF-h5diff_402
    H5DIFF-h5diff_403
    H5DIFF-h5diff_404
    H5DIFF-h5diff_405
    H5DIFF-h5diff_406
    H5DIFF-h5diff_407
    H5DIFF-h5diff_408
    H5DIFF-h5diff_409
    H5DIFF-h5diff_410
    H5DIFF-h5diff_411
    H5DIFF-h5diff_412
    H5DIFF-h5diff_413
    H5DIFF-h5diff_414
    H5DIFF-h5diff_415
    H5DIFF-h5diff_416
    H5DIFF-h5diff_417
    H5DIFF-h5diff_418
    H5DIFF-h5diff_419
    H5DIFF-h5diff_420
    H5DIFF-h5diff_421
    H5DIFF-h5diff_422
    H5DIFF-h5diff_423
    H5DIFF-h5diff_424
    H5DIFF-h5diff_425
    H5DIFF-h5diff_450
    H5DIFF-h5diff_451
    H5DIFF-h5diff_452
    H5DIFF-h5diff_453
    H5DIFF-h5diff_454
    H5DIFF-h5diff_455
    H5DIFF-h5diff_456
    H5DIFF-h5diff_457
    H5DIFF-h5diff_458
    H5DIFF-h5diff_459
    H5DIFF-h5diff_500
    H5DIFF-h5diff_501
    H5DIFF-h5diff_502
    H5DIFF-h5diff_503
    H5DIFF-h5diff_504
    H5DIFF-h5diff_505
    H5DIFF-h5diff_506
    H5DIFF-h5diff_507
    H5DIFF-h5diff_508
    H5DIFF-h5diff_509
    H5DIFF-h5diff_510
    H5DIFF-h5diff_511
    H5DIFF-h5diff_512
    H5DIFF-h5diff_513
    H5DIFF-h5diff_514
    H5DIFF-h5diff_515
    H5DIFF-h5diff_516
    H5DIFF-h5diff_517
    H5DIFF-h5diff_518
    H5DIFF-h5diff_480
    H5DIFF-h5diff_481
    H5DIFF-h5diff_482
    H5DIFF-h5diff_483
    H5DIFF-h5diff_484
    #H5LS-help-1
    #H5LS-help-2
    H5LS-help-3
    #H5LS-tall-1
    H5LS-tall-2
    H5LS-tgroup
    H5LS-tgroup-3
    H5LS-tgroup-1
    H5LS-tgroup-2
    H5LS-tdset-1
    H5LS-tslink-1
    H5LS-tsoftlinks-1
    H5LS-tsoftlinks-2
    H5LS-tsoftlinks-3
    H5LS-tsoftlinks-4
    H5LS-tsoftlinks-5
    H5LS-textlink-1
    H5LS-textlinksrc-1
    H5LS-textlinksrc-2
    H5LS-textlinksrc-3
    H5LS-textlinksrc-4
    H5LS-textlinksrc-5
    H5LS-textlinksrc-6
    H5LS-textlinksrc-7
    H5LS-tudlink-1
    H5LS-textlinksrc-1-old
    H5LS-textlinksrc-2-old
    H5LS-textlinksrc-3-old
    H5LS-textlinksrc-6-old
    H5LS-textlinksrc-7-old
    H5LS-textlinksrc-nodangle-1
    H5LS-textlinksrc-nodangle-2
    H5LS-tsoftlinks-nodangle-1
    H5LS-thlinks-nodangle-1
    H5LS-thlink-1
    H5LS-tcomp-1
    H5LS-tnestcomp-1
    H5LS-tloop-1
    H5LS-tstr-1
    H5LS-tsaf
    H5LS-tvldtypes1
    H5LS-tarray1
    H5LS-tempty
    H5LS-tattr2
    H5LS-nosuchfile
    H5LS-tvldtypes2le
    H5LS-tdataregle
    H5MKGRP-clear-h5mkgrp_help-h
    H5MKGRP-H5LS-h5mkgrp_help-h
    H5MKGRP-clear-h5mkgrp_version-V
    H5MKGRP-H5LS-h5mkgrp_version-V
    H5MKGRP-clear-h5mkgrp_single
    #H5MKGRP-h5mkgrp_single
    #H5MKGRP-H5LS-h5mkgrp_single
    H5MKGRP-clear-h5mkgrp_single-v
    H5MKGRP-h5mkgrp_single-v
    H5MKGRP-H5LS-h5mkgrp_single-v
    H5MKGRP-clear-h5mkgrp_single-p
    H5MKGRP-h5mkgrp_single-p
    H5MKGRP-H5LS-h5mkgrp_single-p
    H5MKGRP-clear-h5mkgrp_single_latest-l
    H5MKGRP-h5mkgrp_single_latest-l
    H5MKGRP-H5LS-h5mkgrp_single_latest-l
    H5MKGRP-clear-h5mkgrp_several
    H5MKGRP-h5mkgrp_several
    H5MKGRP-H5LS-h5mkgrp_several
    H5MKGRP-clear-h5mkgrp_several-v
    H5MKGRP-h5mkgrp_several-v
    H5MKGRP-H5LS-h5mkgrp_several-v
    H5MKGRP-clear-h5mkgrp_several-p
    H5MKGRP-h5mkgrp_several-p
    H5MKGRP-H5LS-h5mkgrp_several-p
    H5MKGRP-clear-h5mkgrp_several_latest-l
    H5MKGRP-h5mkgrp_several_latest-l
    H5MKGRP-H5LS-h5mkgrp_several_latest-l
    H5MKGRP-clear-h5mkgrp_nested-p
    H5MKGRP-h5mkgrp_nested-p
    H5MKGRP-H5LS-h5mkgrp_nested-p
    H5MKGRP-clear-h5mkgrp_nested_latest-lp
    H5MKGRP-h5mkgrp_nested_latest-lp
    H5MKGRP-H5LS-h5mkgrp_nested_latest-lp
    H5MKGRP-clear-h5mkgrp_nested_mult-p
    H5MKGRP-h5mkgrp_nested_mult-p
    H5MKGRP-H5LS-h5mkgrp_nested_mult-p
    H5MKGRP-clear-h5mkgrp_nested_mult_latest-lp
    H5MKGRP-h5mkgrp_nested_mult_latest-lp
    H5MKGRP-H5LS-h5mkgrp_nested_mult_latest-lp
    #H5REPACK_VERIFY_LAYOUT-dset2_chunk_20x10
    H5REPACK_VERIFY_LAYOUT_ALL-chunk_20x10
    H5REPACK_VERIFY_LAYOUT-dset2_conti
    H5REPACK_VERIFY_LAYOUT_ALL-conti
    H5REPACK_VERIFY_LAYOUT-dset2_compa
    H5REPACK_VERIFY_LAYOUT_ALL-compa
    #H5REPACK_VERIFY_LAYOUT-dset_compa_conti
    H5REPACK_VERIFY_LAYOUT-dset_compa_chunk
    H5REPACK_VERIFY_LAYOUT-dset_compa_compa
    H5REPACK_VERIFY_LAYOUT-dset_conti_compa
    H5REPACK_VERIFY_LAYOUT-dset_conti_chunk
    H5REPACK_VERIFY_LAYOUT-dset_conti_conti
    H5REPACK_VERIFY_LAYOUT-chunk_compa
    H5REPACK_VERIFY_LAYOUT-chunk_conti
    H5REPACK_VERIFY_LAYOUT-chunk_18x13
    H5REPACK_VERIFY_LAYOUT-contig_small_compa
    H5REPACK_VERIFY_LAYOUT-contig_small_fixed_compa
    H5REPACK_VERIFY_LAYOUT_ALL-layout_long_switches
    H5REPACK_VERIFY_LAYOUT_ALL-layout_short_switches                
    #H5STAT-h5stat_help1
    H5STAT-h5stat_help2
    H5STAT-h5stat_filters
    H5STAT-h5stat_filters-file
    H5STAT-h5stat_filters-F
    H5STAT-h5stat_filters-d
    H5STAT-h5stat_filters-g
    H5STAT-h5stat_filters-dT
    H5STAT-h5stat_filters-UD
    H5STAT-h5stat_filters-UT
    H5STAT-h5stat_tsohm
    H5STAT-h5stat_newgrat
    H5STAT-h5stat_newgrat-UG
    H5STAT-h5stat_newgrat-UA                    
    H5DUMP-tgroup-1
    H5DUMP-tgroup-2
    #H5DUMP-tdset-1
    #H5DUMP-tdset-2
    H5DUMP-tattr-1
    H5DUMP-tattr-2
    H5DUMP-tattr-3
    H5DUMP-tnamed_dtype_attr
    H5DUMP-tslink-1
    H5DUMP-tudlink-1
    H5DUMP-tslink-2
    H5DUMP-tudlink-2
    H5DUMP-thlink-1
    H5DUMP-thlink-2
    H5DUMP-thlink-3
    H5DUMP-thlink-4
    H5DUMP-thlink-5
    H5DUMP-tcomp-1
    H5DUMP-tcomp-2
    H5DUMP-tcomp-4
    H5DUMP-tnestcomp-1
    H5DUMP-tall-1
    H5DUMP-tall-2
    H5DUMP-tall-3
    H5DUMP-tloop-1
    H5DUMP-tstr-1
    H5DUMP-tstr-2
    H5DUMP-tsaf
    H5DUMP-tvldtypes1
    H5DUMP-tvldtypes2
    H5DUMP-tvldtypes3
    H5DUMP-tvldtypes4
    H5DUMP-tvldtypes5
    H5DUMP-tvlstr
    H5DUMP-tarray1
    H5DUMP-tarray2
    H5DUMP-tarray3
    H5DUMP-tarray4
    H5DUMP-tarray5
    H5DUMP-tarray6
    H5DUMP-tarray7
    H5DUMP-tarray8
    H5DUMP-tempty
    H5DUMP-tgrp_comments
    H5DUMP-tsplit_file
    H5DUMP-tfamily
    H5DUMP-tmulti
    H5DUMP-tlarge_objname
    H5DUMP-tall-2A
    H5DUMP-tall-2B
    H5DUMP-tall-4s
    H5DUMP-tall-5s
    H5DUMP-tdset-3s
    H5DUMP-tchar1
    H5DUMP-tboot1
    H5DUMP-tboot2
    H5DUMP-tperror
    H5DUMP-tcontents
    H5DUMP-tcompact
    H5DUMP-tcontiguos
    H5DUMP-tchunked
    H5DUMP-texternal
    H5DUMP-tfill
    H5DUMP-treference
    H5DUMP-tstringe
    H5DUMP-tstring
    H5DUMP-tstring2
    H5DUMP-tindicesyes
    H5DUMP-tindicesno
    H5DUMP-tindicessub1
    H5DUMP-tindicessub2
    H5DUMP-tindicessub3
    H5DUMP-tindicessub4
    H5DUMP-tszip
    H5DUMP-tdeflate
    H5DUMP-tshuffle
    H5DUMP-tfletcher32
    H5DUMP-tnbit
    H5DUMP-tscaleoffset
    H5DUMP-tallfilters
    H5DUMP-tuserfilter
    H5DUMP-tlonglinks
    H5DUMP-tbigdims
    H5DUMP-thyperslab
    H5DUMP-tnullspace
    H5DUMP-tvms
    H5DUMP-tbin1LE
    H5DUMP-tbin1
    #H5DUMP-h5import-out1
    H5DUMP-tbin2
    H5DUMP-tbin3
    H5DUMP-h5import-out3
    H5DUMP-tbin4
    H5DUMP-tdatareg
    H5DUMP-tdataregR
    H5DUMP-tattrreg
    H5DUMP-tattrregR
    #H5DUMP-output-tbinregR
    #H5DUMP-output-cmp-tbinregR
    H5DUMP-tordergr1
    H5DUMP-tordergr2
    H5DUMP-tordergr3
    H5DUMP-tordergr4
    H5DUMP-tordergr5
    H5DUMP-torderattr1
    H5DUMP-torderattr2
    H5DUMP-torderattr3
    H5DUMP-torderattr4
    H5DUMP-tfpformat
    H5DUMP-textlinksrc
    H5DUMP-textlinkfar
    H5DUMP-tnofilename-with-packed-bits
    H5DUMP-tpbitsSigned
    H5DUMP-tpbitsUnsigned
    H5DUMP-tpbitsOverlapped
    H5DUMP-tpbitsMax
    H5DUMP-tpbitsCompound
    H5DUMP-tpbitsArray
    H5DUMP-tpbitsMaxExceeded
    H5DUMP-tpbitsOffsetExceeded
    H5DUMP-tpbitsOffsetNegative
    H5DUMP-tpbitsLengthPositive
    H5DUMP-tpbitsLengthExceeded
    H5DUMP-tpbitsIncomplete
    H5DUMP-XML-tall.h5
    H5DUMP-XML-tattr.h5
    H5DUMP-XML-tbitfields.h5
    H5DUMP-XML-tcompound.h5
    H5DUMP-XML-tcompound2.h5
    H5DUMP-XML-tdatareg.h5
    H5DUMP-XML-tdset.h5
    H5DUMP-XML-tdset2.h5
    H5DUMP-XML-tenum.h5
    H5DUMP-XML-tgroup.h5
    H5DUMP-XML-thlink.h5
    H5DUMP-XML-tloop.h5
    H5DUMP-XML-tloop2.h5
    H5DUMP-XML-tmany.h5
    H5DUMP-XML-tnestedcomp.h5
    H5DUMP-XML-tcompound_complex.h5
    H5DUMP-XML-tobjref.h5
    H5DUMP-XML-topaque.h5
    H5DUMP-XML-tslink.h5
    H5DUMP-XML-tudlink.h5
    H5DUMP-XML-textlink.h5
    H5DUMP-XML-tstr.h5
    H5DUMP-XML-tstr2.h5
    H5DUMP-XML-tref.h5
    H5DUMP-XML-tname-amp.h5
    H5DUMP-XML-tname-apos.h5
    H5DUMP-XML-tname-gt.h5
    H5DUMP-XML-tname-lt.h5
    H5DUMP-XML-tname-quot.h5
    H5DUMP-XML-tname-sp.h5
    H5DUMP-XML-tstring.h5
    H5DUMP-XML-tstring-at.h5
    H5DUMP-XML-tref-escapes.h5
    H5DUMP-XML-tref-escapes-at.h5
    H5DUMP-XML-tnodata.h5
    H5DUMP-XML-tarray1.h5
    H5DUMP-XML-tarray2.h5
    H5DUMP-XML-tarray3.h5
    H5DUMP-XML-tarray6.h5
    H5DUMP-XML-tarray7.h5
    H5DUMP-XML-tvldtypes1.h5
    H5DUMP-XML-tvldtypes2.h5
    H5DUMP-XML-tvldtypes3.h5
    H5DUMP-XML-tvldtypes4.h5
    H5DUMP-XML-tvldtypes5.h5
    H5DUMP-XML-tvlstr.h5
    H5DUMP-XML-tsaf.h5
    H5DUMP-XML-tempty.h5
    H5DUMP-XML-tnamed_dtype_attr.h5
    H5DUMP-XML-tempty-dtd.h5
    H5DUMP-XML-tempty-dtd-2.h5
    H5DUMP-XML-tempty-nons.h5
    H5DUMP-XML-tempty-nons-2.h5
    H5DUMP-XML-tempty-ns.h5
    H5DUMP-XML-tempty-ns-2.h5
    H5DUMP-XML-tempty-nons-uri.h5
    H5DUMP-XML-tempty-dtd-uri.h5
    H5DUMP-XML-tall-2A.h5
    H5DUMP-XML-torderattr1.h5
    H5DUMP-XML-torderattr2.h5
    H5DUMP-XML-torderattr3.h5
    H5DUMP-XML-torderattr4.h5
    H5DUMP-XML-tfpformat.h5
)
