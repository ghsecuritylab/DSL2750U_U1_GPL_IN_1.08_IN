athcfg wifi0 vapcreate set ath0 infra-sta 1
athcfg ath0 mode set 11NG
athcfg ath0 param set bg-scan 0
athcfg ath0 param set scancash-thres 120
athcfg wifi0 fftxq-min set 1000
athcfg ath0 param set half-gi 1
athcfg ath0 param set cwm-mode 1
#./athcfg ath0 param set cwm-mode 0
athcfg ath0 param set ext-chan -1
athcfg ath0 param set extchan-protspace 0
#./athcfg ath0 param set 11n-ampdu 0
athcfg ath0 param set 11n-ampdu 1
athcfg ath0 param set txchan-mask 1
athcfg ath0 param set rxchan-mask 1
athcfg ath0 freq set 15 0
athcfg ath0 param set wds-enable 1
brctl addif br0 ath0 
ifconfig ath0 up

