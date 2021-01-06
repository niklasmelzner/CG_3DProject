import pygame
from pygame.locals import *

#im=pygame.image.load("2_asteroid_01.bmp")

#sx,sy=im.get_size()

#im2=pygame.Surface((sx/8,sy/8))

#im2.blit(im,(0,0))


#pygame.image.save(im2,"asteroid_01.bmp")

txt="""
vt 0.120677 0.683115
vt 0.056064 0.668699
vt 0.093631 0.635785
vt 0.093631 0.635785
vt 0.138528 0.632510
vt 0.120677 0.683115
vt 0.087979 0.741313
vt 0.032103 0.731682
vt 0.056064 0.668699
vt 0.056064 0.668699
vt 0.120677 0.683115
vt 0.087979 0.741313
vt 0.070206 0.816331
vt 0.025000 0.799627
vt 0.032103 0.731682
vt 0.032103 0.731682
vt 0.087979 0.741313
vt 0.070206 0.816331
vt 0.078593 0.884589
vt 0.032573 0.859139
vt 0.025000 0.799627
vt 0.025000 0.799627
vt 0.070206 0.816331
vt 0.078593 0.884589
vt 0.079600 0.924623
vt 0.039415 0.904170
vt 0.032573 0.859139
vt 0.032573 0.859139
vt 0.078593 0.884589
vt 0.079600 0.924623
vt 0.183785 0.676029
vt 0.120677 0.683115
vt 0.138528 0.632510
vt 0.138528 0.632510
vt 0.201572 0.623658
vt 0.183785 0.676029
vt 0.155222 0.745009
vt 0.087979 0.741313
vt 0.120677 0.683115
vt 0.120677 0.683115
vt 0.183785 0.676029
vt 0.155222 0.745009
vt 0.140422 0.838714
vt 0.070206 0.816331
vt 0.087979 0.741313
vt 0.087979 0.741313
vt 0.155222 0.745009
vt 0.140422 0.838714
vt 0.135211 0.903466
vt 0.078593 0.884589
vt 0.070206 0.816331
vt 0.070206 0.816331
vt 0.140422 0.838714
vt 0.135211 0.903466
vt 0.129683 0.940314
vt 0.079600 0.924623
vt 0.078593 0.884589
vt 0.078593 0.884589
vt 0.135211 0.903466
vt 0.129683 0.940314
vt 0.267736 0.687284
vt 0.183785 0.676029
vt 0.201572 0.623658
vt 0.201572 0.623658
vt 0.269882 0.635849
vt 0.267736 0.687284
vt 0.243071 0.771463
vt 0.155222 0.745009
vt 0.183785 0.676029
vt 0.183785 0.676029
vt 0.267736 0.687284
vt 0.243071 0.771463
vt 0.216127 0.856258
vt 0.140422 0.838714
vt 0.155222 0.745009
vt 0.155222 0.745009
vt 0.243071 0.771463
vt 0.216127 0.856258
vt 0.201466 0.916383
vt 0.135211 0.903466
vt 0.140422 0.838714
vt 0.140422 0.838714
vt 0.216127 0.856258
vt 0.201466 0.916383
vt 0.191472 0.975000
vt 0.129683 0.940314
vt 0.135211 0.903466
vt 0.135211 0.903466
vt 0.201466 0.916383
vt 0.191472 0.975000
vt 0.329639 0.715984
vt 0.267736 0.687284
vt 0.269882 0.635849
vt 0.269882 0.635849
vt 0.326091 0.668579
vt 0.329639 0.715984
vt 0.334702 0.793407
vt 0.243071 0.771463
vt 0.267736 0.687284
vt 0.267736 0.687284
vt 0.329639 0.715984
vt 0.334702 0.793407
vt 0.298930 0.874902
vt 0.216127 0.856258
vt 0.243071 0.771463
vt 0.243071 0.771463
vt 0.334702 0.793407
vt 0.298930 0.874902
vt 0.271719 0.934702
vt 0.201466 0.916383
vt 0.216127 0.856258
vt 0.216127 0.856258
vt 0.298930 0.874902
vt 0.271719 0.934702
vt 0.242753 0.969628
vt 0.191472 0.975000
vt 0.201466 0.916383
vt 0.201466 0.916383
vt 0.271719 0.934702
vt 0.242753 0.969628
vt 0.378061 0.739424
vt 0.329639 0.715984
vt 0.326091 0.668579
vt 0.326091 0.668579
vt 0.367725 0.689907
vt 0.378061 0.739424
vt 0.384057 0.811860
vt 0.334702 0.793407
vt 0.329639 0.715984
vt 0.329639 0.715984
vt 0.378061 0.739424
vt 0.384057 0.811860
vt 0.380033 0.890012
vt 0.298930 0.874902
vt 0.334702 0.793407
vt 0.334702 0.793407
vt 0.384057 0.811860
vt 0.380033 0.890012
vt 0.327075 0.935884
vt 0.271719 0.934702
vt 0.298930 0.874902
vt 0.298930 0.874902
vt 0.380033 0.890012
vt 0.327075 0.935884
vt 0.289771 0.972112
vt 0.242753 0.969628
vt 0.271719 0.934702
vt 0.271719 0.934702
vt 0.327075 0.935884
vt 0.289771 0.972112
vt 0.478280 0.709323
vt 0.436861 0.707725
vt 0.449573 0.670601
vt 0.449573 0.670601
vt 0.485897 0.662047
vt 0.478280 0.709323
vt 0.528456 0.697388
vt 0.478280 0.709323
vt 0.485897 0.662047
vt 0.485897 0.662047
vt 0.536514 0.652795
vt 0.528456 0.697388
vt 0.609412 0.698574
vt 0.528456 0.697388
vt 0.536514 0.652795
vt 0.536514 0.652795
vt 0.600284 0.650905
vt 0.609412 0.698574
vt 0.681597 0.713364
vt 0.609412 0.698574
vt 0.600284 0.650905
vt 0.600284 0.650905
vt 0.677401 0.657515
vt 0.681597 0.713364
vt 0.729537 0.749308
vt 0.681597 0.713364
vt 0.677401 0.657515
vt 0.677401 0.657515
vt 0.725242 0.682458
vt 0.729537 0.749308
vt 0.459262 0.758895
vt 0.404550 0.756301
vt 0.436861 0.707725
vt 0.436861 0.707725
vt 0.478280 0.709323
vt 0.459262 0.758895
vt 0.519389 0.765901
vt 0.459262 0.758895
vt 0.478280 0.709323
vt 0.478280 0.709323
vt 0.528456 0.697388
vt 0.519389 0.765901
vt 0.620001 0.776784
vt 0.519389 0.765901
vt 0.528456 0.697388
vt 0.528456 0.697388
vt 0.609412 0.698574
vt 0.620001 0.776784
vt 0.688690 0.791926
vt 0.620001 0.776784
vt 0.609412 0.698574
vt 0.609412 0.698574
vt 0.681597 0.713364
vt 0.688690 0.791926
vt 0.730717 0.807678
vt 0.688690 0.791926
vt 0.681597 0.713364
vt 0.681597 0.713364
vt 0.729537 0.749308
vt 0.730717 0.807678
vt 0.438805 0.837416
vt 0.399756 0.820208
vt 0.404550 0.756301
vt 0.404550 0.756301
vt 0.459262 0.758895
vt 0.438805 0.837416
vt 0.516662 0.852762
vt 0.438805 0.837416
vt 0.459262 0.758895
vt 0.459262 0.758895
vt 0.519389 0.765901
vt 0.516662 0.852762
vt 0.603294 0.869848
vt 0.516662 0.852762
vt 0.519389 0.765901
vt 0.519389 0.765901
vt 0.620001 0.776784
vt 0.603294 0.869848
vt 0.672951 0.866919
vt 0.603294 0.869848
vt 0.620001 0.776784
vt 0.620001 0.776784
vt 0.688690 0.791926
vt 0.672951 0.866919
vt 0.719310 0.870585
vt 0.672951 0.866919
vt 0.688690 0.791926
vt 0.688690 0.791926
vt 0.730717 0.807678
vt 0.719310 0.870585
vt 0.445111 0.903164
vt 0.400781 0.883566
vt 0.399756 0.820208
vt 0.399756 0.820208
vt 0.438805 0.837416
vt 0.445111 0.903164
vt 0.513055 0.924747
vt 0.445111 0.903164
vt 0.438805 0.837416
vt 0.438805 0.837416
vt 0.516662 0.852762
vt 0.513055 0.924747
vt 0.586584 0.930759
vt 0.513055 0.924747
vt 0.516662 0.852762
vt 0.516662 0.852762
vt 0.603294 0.869848
vt 0.586584 0.930759
vt 0.650024 0.925254
vt 0.586584 0.930759
vt 0.603294 0.869848
vt 0.603294 0.869848
vt 0.672951 0.866919
vt 0.650024 0.925254
vt 0.693146 0.919269
vt 0.650024 0.925254
vt 0.672951 0.866919
vt 0.672951 0.866919
vt 0.719310 0.870585
vt 0.693146 0.919269
vt 0.456796 0.956356
vt 0.413689 0.931083
vt 0.400781 0.883566
vt 0.400781 0.883566
vt 0.445111 0.903164
vt 0.456796 0.956356
vt 0.516044 0.975000
vt 0.456796 0.956356
vt 0.445111 0.903164
vt 0.445111 0.903164
vt 0.513055 0.924747
vt 0.516044 0.975000
vt 0.579118 0.973501
vt 0.516044 0.975000
vt 0.513055 0.924747
vt 0.513055 0.924747
vt 0.586584 0.930759
vt 0.579118 0.973501
vt 0.634225 0.964067
vt 0.579118 0.973501
vt 0.586584 0.930759
vt 0.586584 0.930759
vt 0.650024 0.925254
vt 0.634225 0.964067
vt 0.673143 0.953326
vt 0.634225 0.964067
vt 0.650024 0.925254
vt 0.650024 0.925254
vt 0.693146 0.919269
vt 0.673143 0.953326
vt 0.098213 0.251569
vt 0.107732 0.294901
vt 0.059059 0.293041
vt 0.059059 0.293041
vt 0.039502 0.249831
vt 0.098213 0.251569
vt 0.165094 0.265931
vt 0.171062 0.302816
vt 0.107732 0.294901
vt 0.107732 0.294901
vt 0.098213 0.251569
vt 0.165094 0.265931
vt 0.239824 0.250591
vt 0.239775 0.298326
vt 0.171062 0.302816
vt 0.171062 0.302816
vt 0.165094 0.265931
vt 0.239824 0.250591
vt 0.304012 0.248038
vt 0.300975 0.298743
vt 0.239775 0.298326
vt 0.239775 0.298326
vt 0.239824 0.250591
vt 0.304012 0.248038
vt 0.350832 0.244819
vt 0.338337 0.285856
vt 0.300975 0.298743
vt 0.300975 0.298743
vt 0.304012 0.248038
vt 0.350832 0.244819
vt 0.085577 0.193477
vt 0.098213 0.251569
vt 0.039502 0.249831
vt 0.039502 0.249831
vt 0.025000 0.194964
vt 0.085577 0.193477
vt 0.157925 0.193131
vt 0.165094 0.265931
vt 0.098213 0.251569
vt 0.098213 0.251569
vt 0.085577 0.193477
vt 0.157925 0.193131
vt 0.238955 0.193197
vt 0.239824 0.250591
vt 0.165094 0.265931
vt 0.165094 0.265931
vt 0.157925 0.193131
vt 0.238955 0.193197
vt 0.311703 0.190072
vt 0.304012 0.248038
vt 0.239824 0.250591
vt 0.239824 0.250591
vt 0.238955 0.193197
vt 0.311703 0.190072
vt 0.361809 0.195941
vt 0.350832 0.244819
vt 0.304012 0.248038
vt 0.304012 0.248038
vt 0.311703 0.190072
vt 0.361809 0.195941
vt 0.083390 0.135914
vt 0.085577 0.193477
vt 0.025000 0.194964
vt 0.025000 0.194964
vt 0.031678 0.130999
vt 0.083390 0.135914
vt 0.157719 0.133317
vt 0.157925 0.193131
vt 0.085577 0.193477
vt 0.085577 0.193477
vt 0.083390 0.135914
vt 0.157719 0.133317
vt 0.241966 0.134743
vt 0.238955 0.193197
vt 0.157925 0.193131
vt 0.157925 0.193131
vt 0.157719 0.133317
vt 0.241966 0.134743
vt 0.313702 0.131133
vt 0.311703 0.190072
vt 0.238955 0.193197
vt 0.238955 0.193197
vt 0.241966 0.134743
vt 0.313702 0.131133
vt 0.366900 0.136129
vt 0.361809 0.195941
vt 0.311703 0.190072
vt 0.311703 0.190072
vt 0.313702 0.131133
vt 0.366900 0.136129
vt 0.088976 0.075998
vt 0.083390 0.135914
vt 0.031678 0.130999
vt 0.031678 0.130999
vt 0.045566 0.081960
vt 0.088976 0.075998
vt 0.158896 0.070723
vt 0.157719 0.133317
vt 0.083390 0.135914
vt 0.083390 0.135914
vt 0.088976 0.075998
vt 0.158896 0.070723
vt 0.246893 0.080166
vt 0.241966 0.134743
vt 0.157719 0.133317
vt 0.157719 0.133317
vt 0.158896 0.070723
vt 0.246893 0.080166
vt 0.310232 0.080382
vt 0.313702 0.131133
vt 0.241966 0.134743
vt 0.241966 0.134743
vt 0.246893 0.080166
vt 0.310232 0.080382
vt 0.361837 0.086649
vt 0.366900 0.136129
vt 0.313702 0.131133
vt 0.313702 0.131133
vt 0.310232 0.080382
vt 0.361837 0.086649
vt 0.097042 0.034665
vt 0.088976 0.075998
vt 0.045566 0.081960
vt 0.045566 0.081960
vt 0.052639 0.047412
vt 0.097042 0.034665
vt 0.162185 0.026969
vt 0.158896 0.070723
vt 0.088976 0.075998
vt 0.088976 0.075998
vt 0.097042 0.034665
vt 0.162185 0.026969
vt 0.240463 0.025000
vt 0.246893 0.080166
vt 0.158896 0.070723
vt 0.158896 0.070723
vt 0.162185 0.026969
vt 0.240463 0.025000
vt 0.310141 0.037081
vt 0.310232 0.080382
vt 0.246893 0.080166
vt 0.246893 0.080166
vt 0.240463 0.025000
vt 0.310141 0.037081
vt 0.354536 0.049702
vt 0.361837 0.086649
vt 0.310232 0.080382
vt 0.310232 0.080382
vt 0.310141 0.037081
vt 0.354536 0.049702
vt 0.444373 0.248861
vt 0.464457 0.300791
vt 0.396953 0.283585
vt 0.396953 0.283585
vt 0.382729 0.236278
vt 0.444373 0.248861
vt 0.519157 0.254467
vt 0.526685 0.302816
vt 0.464457 0.300791
vt 0.464457 0.300791
vt 0.444373 0.248861
vt 0.519157 0.254467
vt 0.591861 0.249034
vt 0.594288 0.295201
vt 0.526685 0.302816
vt 0.526685 0.302816
vt 0.519157 0.254467
vt 0.591861 0.249034
vt 0.657911 0.247827
vt 0.650152 0.288580
vt 0.594288 0.295201
vt 0.594288 0.295201
vt 0.591861 0.249034
vt 0.657911 0.247827
vt 0.699111 0.245044
vt 0.688593 0.283784
vt 0.650152 0.288580
vt 0.650152 0.288580
vt 0.657911 0.247827
vt 0.699111 0.245044
vt 0.437887 0.189644
vt 0.444373 0.248861
vt 0.382729 0.236278
vt 0.382729 0.236278
vt 0.382599 0.180492
vt 0.437887 0.189644
vt 0.512378 0.199458
vt 0.519157 0.254467
vt 0.444373 0.248861
vt 0.444373 0.248861
vt 0.437887 0.189644
vt 0.512378 0.199458
vt 0.596803 0.196238
vt 0.591861 0.249034
vt 0.519157 0.254467
vt 0.519157 0.254467
vt 0.512378 0.199458
vt 0.596803 0.196238
vt 0.662137 0.197493
vt 0.657911 0.247827
vt 0.591861 0.249034
vt 0.591861 0.249034
vt 0.596803 0.196238
vt 0.662137 0.197493
vt 0.707490 0.197703
vt 0.699111 0.245044
vt 0.657911 0.247827
vt 0.657911 0.247827
vt 0.662137 0.197493
vt 0.707490 0.197703
vt 0.443000 0.129665
vt 0.437887 0.189644
vt 0.382599 0.180492
vt 0.382599 0.180492
vt 0.390537 0.121897
vt 0.443000 0.129665
vt 0.518084 0.125302
vt 0.512378 0.199458
vt 0.437887 0.189644
vt 0.437887 0.189644
vt 0.443000 0.129665
vt 0.518084 0.125302
vt 0.592746 0.137525
vt 0.596803 0.196238
vt 0.512378 0.199458
vt 0.512378 0.199458
vt 0.518084 0.125302
vt 0.592746 0.137525
vt 0.659726 0.141520
vt 0.662137 0.197493
vt 0.596803 0.196238
vt 0.596803 0.196238
vt 0.592746 0.137525
vt 0.659726 0.141520
vt 0.706466 0.144396
vt 0.707490 0.197703
vt 0.662137 0.197493
vt 0.662137 0.197493
vt 0.659726 0.141520
vt 0.706466 0.144396
vt 0.452688 0.075804
vt 0.443000 0.129665
vt 0.390537 0.121897
vt 0.390537 0.121897
vt 0.408107 0.064792
vt 0.452688 0.075804
vt 0.520200 0.075851
vt 0.518084 0.125302
vt 0.443000 0.129665
vt 0.443000 0.129665
vt 0.452688 0.075804
vt 0.520200 0.075851
vt 0.590527 0.077354
vt 0.592746 0.137525
vt 0.518084 0.125302
vt 0.518084 0.125302
vt 0.520200 0.075851
vt 0.590527 0.077354
vt 0.652436 0.087922
vt 0.659726 0.141520
vt 0.592746 0.137525
vt 0.592746 0.137525
vt 0.590527 0.077354
vt 0.652436 0.087922
vt 0.705473 0.097036
vt 0.706466 0.144396
vt 0.659726 0.141520
vt 0.659726 0.141520
vt 0.652436 0.087922
vt 0.705473 0.097036
vt 0.467459 0.032585
vt 0.452688 0.075804
vt 0.408107 0.064792
vt 0.408107 0.064792
vt 0.422698 0.036533
vt 0.467459 0.032585
vt 0.524033 0.026160
vt 0.520200 0.075851
vt 0.452688 0.075804
vt 0.452688 0.075804
vt 0.467459 0.032585
vt 0.524033 0.026160
vt 0.595860 0.039266
vt 0.590527 0.077354
vt 0.520200 0.075851
vt 0.520200 0.075851
vt 0.524033 0.026160
vt 0.595860 0.039266
vt 0.647379 0.052543
vt 0.652436 0.087922
vt 0.590527 0.077354
vt 0.590527 0.077354
vt 0.595860 0.039266
vt 0.647379 0.052543
vt 0.692481 0.060001
vt 0.705473 0.097036
vt 0.652436 0.087922
vt 0.652436 0.087922
vt 0.647379 0.052543
vt 0.692481 0.060001
vt 0.452633 0.557136
vt 0.457405 0.595098
vt 0.422142 0.590437
vt 0.422142 0.590437
vt 0.406541 0.552296
vt 0.452633 0.557136
vt 0.512933 0.556126
vt 0.508699 0.602113
vt 0.457405 0.595098
vt 0.457405 0.595098
vt 0.452633 0.557136
vt 0.512933 0.556126
vt 0.587629 0.561916
vt 0.574821 0.607960
vt 0.508699 0.602113
vt 0.508699 0.602113
vt 0.512933 0.556126
vt 0.587629 0.561916
vt 0.671644 0.548240
vt 0.655171 0.597830
vt 0.574821 0.607960
vt 0.574821 0.607960
vt 0.587629 0.561916
vt 0.671644 0.548240
vt 0.730837 0.546441
vt 0.713148 0.594520
vt 0.655171 0.597830
vt 0.655171 0.597830
vt 0.671644 0.548240
vt 0.730837 0.546441
vt 0.446280 0.510128
vt 0.452633 0.557136
vt 0.406541 0.552296
vt 0.406541 0.552296
vt 0.395326 0.505831
vt 0.446280 0.510128
vt 0.509649 0.499758
vt 0.512933 0.556126
vt 0.452633 0.557136
vt 0.452633 0.557136
vt 0.446280 0.510128
vt 0.509649 0.499758
vt 0.602953 0.490303
vt 0.587629 0.561916
vt 0.512933 0.556126
vt 0.512933 0.556126
vt 0.509649 0.499758
vt 0.602953 0.490303
vt 0.675023 0.493274
vt 0.671644 0.548240
vt 0.587629 0.561916
vt 0.587629 0.561916
vt 0.602953 0.490303
vt 0.675023 0.493274
vt 0.734020 0.490266
vt 0.730837 0.546441
vt 0.671644 0.548240
vt 0.671644 0.548240
vt 0.675023 0.493274
vt 0.734020 0.490266
vt 0.439362 0.449692
vt 0.446280 0.510128
vt 0.395326 0.505831
vt 0.395326 0.505831
vt 0.386633 0.447135
vt 0.439362 0.449692
vt 0.515057 0.436650
vt 0.509649 0.499758
vt 0.446280 0.510128
vt 0.446280 0.510128
vt 0.439362 0.449692
vt 0.515057 0.436650
vt 0.595339 0.435279
vt 0.602953 0.490303
vt 0.509649 0.499758
vt 0.509649 0.499758
vt 0.515057 0.436650
vt 0.595339 0.435279
vt 0.672159 0.424837
vt 0.675023 0.493274
vt 0.602953 0.490303
vt 0.602953 0.490303
vt 0.595339 0.435279
vt 0.672159 0.424837
vt 0.725745 0.434033
vt 0.734020 0.490266
vt 0.675023 0.493274
vt 0.675023 0.493274
vt 0.672159 0.424837
vt 0.725745 0.434033
vt 0.444752 0.390424
vt 0.439362 0.449692
vt 0.386633 0.447135
vt 0.386633 0.447135
vt 0.390796 0.398021
vt 0.444752 0.390424
vt 0.504086 0.381417
vt 0.515057 0.436650
vt 0.439362 0.449692
vt 0.439362 0.449692
vt 0.444752 0.390424
vt 0.504086 0.381417
vt 0.595111 0.370276
vt 0.595339 0.435279
vt 0.515057 0.436650
vt 0.515057 0.436650
vt 0.504086 0.381417
vt 0.595111 0.370276
vt 0.660549 0.375822
vt 0.672159 0.424837
vt 0.595339 0.435279
vt 0.595339 0.435279
vt 0.595111 0.370276
vt 0.660549 0.375822
vt 0.703242 0.378553
vt 0.725745 0.434033
vt 0.672159 0.424837
vt 0.672159 0.424837
vt 0.660549 0.375822
vt 0.703242 0.378553
vt 0.444712 0.343798
vt 0.444752 0.390424
vt 0.390796 0.398021
vt 0.390796 0.398021
vt 0.394523 0.358690
vt 0.444712 0.343798
vt 0.517254 0.326640
vt 0.504086 0.381417
vt 0.444752 0.390424
vt 0.444752 0.390424
vt 0.444712 0.343798
vt 0.517254 0.326640
vt 0.591905 0.328842
vt 0.595111 0.370276
vt 0.504086 0.381417
vt 0.504086 0.381417
vt 0.517254 0.326640
vt 0.591905 0.328842
vt 0.651819 0.336700
vt 0.660549 0.375822
vt 0.595111 0.370276
vt 0.595111 0.370276
vt 0.591905 0.328842
vt 0.651819 0.336700
vt 0.696543 0.344936
vt 0.703242 0.378553
vt 0.660549 0.375822
vt 0.660549 0.375822
vt 0.651819 0.336700
vt 0.696543 0.344936
vt 0.076595 0.544490
vt 0.083097 0.589330
vt 0.044228 0.579030
vt 0.044228 0.579030
vt 0.035825 0.538905
vt 0.076595 0.544490
vt 0.153187 0.547226
vt 0.144752 0.607960
vt 0.083097 0.589330
vt 0.083097 0.589330
vt 0.076595 0.544490
vt 0.153187 0.547226
vt 0.225493 0.560106
vt 0.218428 0.607054
vt 0.144752 0.607960
vt 0.144752 0.607960
vt 0.153187 0.547226
vt 0.225493 0.560106
vt 0.295206 0.557568
vt 0.284169 0.601926
vt 0.218428 0.607054
vt 0.218428 0.607054
vt 0.225493 0.560106
vt 0.295206 0.557568
vt 0.348936 0.555981
vt 0.334499 0.601943
vt 0.284169 0.601926
vt 0.284169 0.601926
vt 0.295206 0.557568
vt 0.348936 0.555981
vt 0.066312 0.494054
vt 0.076595 0.544490
vt 0.035825 0.538905
vt 0.035825 0.538905
vt 0.025206 0.491148
vt 0.066312 0.494054
vt 0.152626 0.497271
vt 0.153187 0.547226
vt 0.076595 0.544490
vt 0.076595 0.544490
vt 0.066312 0.494054
vt 0.152626 0.497271
vt 0.236042 0.505596
vt 0.225493 0.560106
vt 0.153187 0.547226
vt 0.153187 0.547226
vt 0.152626 0.497271
vt 0.236042 0.505596
vt 0.311639 0.499736
vt 0.295206 0.557568
vt 0.225493 0.560106
vt 0.225493 0.560106
vt 0.236042 0.505596
vt 0.311639 0.499736
vt 0.368359 0.500029
vt 0.348936 0.555981
vt 0.295206 0.557568
vt 0.295206 0.557568
vt 0.311639 0.499736
vt 0.368359 0.500029
vt 0.077724 0.439365
vt 0.066312 0.494054
vt 0.025206 0.491148
vt 0.025206 0.491148
vt 0.025000 0.440355
vt 0.077724 0.439365
vt 0.150474 0.440501
vt 0.152626 0.497271
vt 0.066312 0.494054
vt 0.066312 0.494054
vt 0.077724 0.439365
vt 0.150474 0.440501
vt 0.239573 0.438380
vt 0.236042 0.505596
vt 0.152626 0.497271
vt 0.152626 0.497271
vt 0.150474 0.440501
vt 0.239573 0.438380
vt 0.316706 0.436962
vt 0.311639 0.499736
vt 0.236042 0.505596
vt 0.236042 0.505596
vt 0.239573 0.438380
vt 0.316706 0.436962
vt 0.367181 0.434004
vt 0.368359 0.500029
vt 0.311639 0.499736
vt 0.311639 0.499736
vt 0.316706 0.436962
vt 0.367181 0.434004
vt 0.085998 0.381783
vt 0.077724 0.439365
vt 0.025000 0.440355
vt 0.025000 0.440355
vt 0.031051 0.387222
vt 0.085998 0.381783
vt 0.156353 0.372856
vt 0.150474 0.440501
vt 0.077724 0.439365
vt 0.077724 0.439365
vt 0.085998 0.381783
vt 0.156353 0.372856
vt 0.242661 0.376521
vt 0.239573 0.438380
vt 0.150474 0.440501
vt 0.150474 0.440501
vt 0.156353 0.372856
vt 0.242661 0.376521
vt 0.320818 0.383674
vt 0.316706 0.436962
vt 0.239573 0.438380
vt 0.239573 0.438380
vt 0.242661 0.376521
vt 0.320818 0.383674
vt 0.370934 0.385706
vt 0.367181 0.434004
vt 0.316706 0.436962
vt 0.316706 0.436962
vt 0.320818 0.383674
vt 0.370934 0.385706
vt 0.078060 0.332680
vt 0.085998 0.381783
vt 0.031051 0.387222
vt 0.031051 0.387222
vt 0.025090 0.344938
vt 0.078060 0.332680
vt 0.157084 0.318515
vt 0.156353 0.372856
vt 0.085998 0.381783
vt 0.085998 0.381783
vt 0.078060 0.332680
vt 0.157084 0.318515
vt 0.246199 0.324581
vt 0.242661 0.376521
vt 0.156353 0.372856
vt 0.156353 0.372856
vt 0.157084 0.318515
vt 0.246199 0.324581
vt 0.314978 0.344087
vt 0.320818 0.383674
vt 0.242661 0.376521
vt 0.242661 0.376521
vt 0.246199 0.324581
vt 0.314978 0.344087
vt 0.363247 0.351622
vt 0.370934 0.385706
vt 0.320818 0.383674
vt 0.320818 0.383674
vt 0.314978 0.344087
vt 0.363247 0.351622
"""

txt1="""
g Surface1
s 1

f 48/1/48 46/2/46 31/3/31 
f 31/4/31 20/5/20 48/6/48 
f 63/7/63 66/8/66 46/9/46 
f 46/10/46 48/11/48 63/12/63 
f 82/13/82 77/14/77 66/15/66 
f 66/16/66 63/17/63 82/18/82 
f 102/19/102 97/20/97 77/21/77 
f 77/22/77 82/23/82 102/24/102 
f 122/25/122 116/26/116 97/27/97 
f 97/28/97 102/29/102 122/30/122 
f 34/31/34 48/32/48 20/33/20 
f 20/34/20 13/35/13 34/36/34 
f 57/37/57 63/38/63 48/39/48 
f 48/40/48 34/41/34 57/42/57 
f 81/43/81 82/44/82 63/45/63 
f 63/46/63 57/47/57 81/48/81 
f 104/49/104 102/50/102 82/51/82 
f 82/52/82 81/53/81 104/54/104 
f 121/55/121 122/56/122 102/57/102 
f 102/58/102 104/59/104 121/60/121 
f 23/61/23 34/62/34 13/63/13 
f 13/64/13 11/65/11 23/66/23 
f 59/67/59 57/68/57 34/69/34 
f 34/70/34 23/71/23 59/72/59 
f 79/73/79 81/74/81 57/75/57 
f 57/76/57 59/77/59 79/78/79 
f 101/79/101 104/80/104 81/81/81 
f 81/82/81 79/83/79 101/84/101 
f 132/85/132 121/86/121 104/87/104 
f 104/88/104 101/89/101 132/90/132 
f 36/91/36 23/92/23 11/93/11 
f 11/94/11 22/95/22 36/96/36 
f 60/97/60 59/98/59 23/99/23 
f 23/100/23 36/101/36 60/102/60 
f 84/103/84 79/104/79 59/105/59 
f 59/106/59 60/107/60 84/108/84 
f 106/109/106 101/110/101 79/111/79 
f 79/112/79 84/113/84 106/114/106 
f 126/115/126 132/116/132 101/117/101 
f 101/118/101 106/119/106 126/120/126 
f 50/121/50 36/122/36 22/123/22 
f 22/124/22 35/125/35 50/126/50 
f 67/127/67 60/128/60 36/129/36 
f 36/130/36 50/131/50 67/132/67 
f 87/133/87 84/134/84 60/135/60 
f 60/136/60 67/137/67 87/138/87 
f 100/139/100 106/140/106 84/141/84 
f 84/142/84 87/143/87 100/144/100 
f 112/145/112 126/146/126 106/147/106 
f 106/148/106 100/149/100 112/150/112 
# num_faces: 50

g Surface2
s 2

f 56/151/56 40/152/40 44/153/44 
f 44/154/44 55/155/55 56/156/56 
f 72/157/72 56/158/56 55/159/55 
f 55/160/55 75/161/75 72/162/72 
f 88/163/88 72/164/72 75/165/75 
f 75/166/75 78/167/78 88/168/88 
f 114/169/114 88/170/88 78/171/78 
f 78/172/78 107/173/107 114/174/114 
f 130/175/130 114/176/114 107/177/107 
f 107/178/107 131/179/131 130/180/130 
f 51/181/51 25/182/25 40/183/40 
f 40/184/40 56/185/56 51/186/51 
f 69/187/69 51/188/51 56/189/56 
f 56/190/56 72/191/72 69/192/69 
f 96/193/96 69/194/69 72/195/72 
f 72/196/72 88/197/88 96/198/96 
f 120/199/120 96/200/96 88/201/88 
f 88/202/88 114/203/114 120/204/120 
f 137/205/137 120/206/120 114/207/114 
f 114/208/114 130/209/130 137/210/137 
f 39/211/39 27/212/27 25/213/25 
f 25/214/25 51/215/51 39/216/39 
f 71/217/71 39/218/39 51/219/51 
f 51/220/51 69/221/69 71/222/71 
f 95/223/95 71/224/71 69/225/69 
f 69/226/69 96/227/96 95/228/95 
f 115/229/115 95/230/95 96/231/96 
f 96/232/96 120/233/120 115/234/115 
f 136/235/136 115/236/115 120/237/120 
f 120/238/120 137/239/137 136/240/136 
f 47/241/47 29/242/29 27/243/27 
f 27/244/27 39/245/39 47/246/47 
f 73/247/73 47/248/47 39/249/39 
f 39/250/39 71/251/71 73/252/73 
f 86/253/86 73/254/73 71/255/71 
f 71/256/71 95/257/95 86/258/86 
f 103/259/103 86/260/86 95/261/95 
f 95/262/95 115/263/115 103/264/103 
f 123/265/123 103/266/103 115/267/115 
f 115/268/115 136/269/136 123/270/123 
f 54/271/54 38/272/38 29/273/29 
f 29/274/29 47/275/47 54/276/54 
f 76/277/76 54/278/54 47/279/47 
f 47/280/47 73/281/73 76/282/76 
f 83/283/83 76/284/76 73/285/73 
f 73/286/73 86/287/86 83/288/83 
f 98/289/98 83/290/83 86/291/86 
f 86/292/86 103/293/103 98/294/98 
f 113/295/113 98/296/98 103/297/103 
f 103/298/103 123/299/123 113/300/113 
# num_faces: 50

g Surface3
s 3

f 16/301/16 29/302/29 38/303/38 
f 38/304/38 26/305/26 16/306/16 
f 17/307/17 27/308/27 29/309/29 
f 29/310/29 16/311/16 17/312/17 
f 10/313/10 25/314/25 27/315/27 
f 27/316/27 17/317/17 10/318/10 
f 21/319/21 40/320/40 25/321/25 
f 25/322/25 10/323/10 21/324/21 
f 33/325/33 44/326/44 40/327/40 
f 40/328/40 21/329/21 33/330/33 
f 7/331/7 16/332/16 26/333/26 
f 26/334/26 15/335/15 7/336/7 
f 3/337/3 17/338/17 16/339/16 
f 16/340/16 7/341/7 3/342/3 
f 4/343/4 10/344/10 17/345/17 
f 17/346/17 3/347/3 4/348/4 
f 12/349/12 21/350/21 10/351/10 
f 10/352/10 4/353/4 12/354/12 
f 30/355/30 33/356/33 21/357/21 
f 21/358/21 12/359/12 30/360/30 
f 8/361/8 7/362/7 15/363/15 
f 15/364/15 24/365/24 8/366/8 
f 1/367/1 3/368/3 7/369/7 
f 7/370/7 8/371/8 1/372/1 
f 5/373/5 4/374/4 3/375/3 
f 3/376/3 1/377/1 5/378/5 
f 9/379/9 12/380/12 4/381/4 
f 4/382/4 5/383/5 9/384/9 
f 19/385/19 30/386/30 12/387/12 
f 12/388/12 9/389/9 19/390/19 
f 18/391/18 8/392/8 24/393/24 
f 24/394/24 32/395/32 18/396/18 
f 6/397/6 1/398/1 8/399/8 
f 8/400/8 18/401/18 6/402/6 
f 2/403/2 5/404/5 1/405/1 
f 1/406/1 6/407/6 2/408/2 
f 14/409/14 9/410/9 5/411/5 
f 5/412/5 2/413/2 14/414/14 
f 28/415/28 19/416/19 9/417/9 
f 9/418/9 14/419/14 28/420/28 
f 22/421/22 18/422/18 32/423/32 
f 32/424/32 35/425/35 22/426/22 
f 11/427/11 6/428/6 18/429/18 
f 18/430/18 22/431/22 11/432/11 
f 13/433/13 2/434/2 6/435/6 
f 6/436/6 11/437/11 13/438/13 
f 20/439/20 14/440/14 2/441/2 
f 2/442/2 13/443/13 20/444/20 
f 31/445/31 28/446/28 14/447/14 
f 14/448/14 20/449/20 31/450/31 
# num_faces: 50

g Surface4
s 4

f 146/451/146 130/452/130 131/453/131 
f 131/454/131 143/455/143 146/456/146 
f 150/457/150 137/458/137 130/459/130 
f 130/460/130 146/461/146 150/462/150 
f 147/463/147 136/464/136 137/465/137 
f 137/466/137 150/467/150 147/468/147 
f 133/469/133 123/470/123 136/471/136 
f 136/472/136 147/473/147 133/474/133 
f 125/475/125 113/476/113 123/477/123 
f 123/478/123 133/479/133 125/480/125 
f 151/481/151 146/482/146 143/483/143 
f 143/484/143 141/485/141 151/486/151 
f 152/487/152 150/488/150 146/489/146 
f 146/490/146 151/491/151 152/492/152 
f 148/493/148 147/494/147 150/495/150 
f 150/496/150 152/497/152 148/498/148 
f 140/499/140 133/500/133 147/501/147 
f 147/502/147 148/503/148 140/504/140 
f 128/505/128 125/506/125 133/507/133 
f 133/508/133 140/509/140 128/510/128 
f 144/511/144 151/512/151 141/513/141 
f 141/514/141 134/515/134 144/516/144 
f 145/517/145 152/518/152 151/519/151 
f 151/520/151 144/521/144 145/522/145 
f 149/523/149 148/524/148 152/525/152 
f 152/526/152 145/527/145 149/528/149 
f 139/529/139 140/530/140 148/531/148 
f 148/532/148 149/533/149 139/534/139 
f 124/535/124 128/536/128 140/537/140 
f 140/538/140 139/539/139 124/540/124 
f 135/541/135 144/542/144 134/543/134 
f 134/544/134 119/545/119 135/546/135 
f 142/547/142 145/548/145 144/549/144 
f 144/550/144 135/551/135 142/552/142 
f 138/553/138 149/554/149 145/555/145 
f 145/556/145 142/557/142 138/558/138 
f 129/559/129 139/560/139 149/561/149 
f 149/562/149 138/563/138 129/564/129 
f 127/565/127 124/566/124 139/567/139 
f 139/568/139 129/569/129 127/570/127 
f 122/571/122 135/572/135 119/573/119 
f 119/574/119 116/575/116 122/576/122 
f 121/577/121 142/578/142 135/579/135 
f 135/580/135 122/581/122 121/582/121 
f 132/583/132 138/584/138 142/585/142 
f 142/586/142 121/587/121 132/588/132 
f 126/589/126 129/590/129 138/591/138 
f 138/592/138 132/593/132 126/594/126 
f 112/595/112 127/596/127 129/597/129 
f 129/598/129 126/599/126 112/600/112 
# num_faces: 50

g Surface5
s 5

f 53/601/53 55/602/55 44/603/44 
f 44/604/44 33/605/33 53/606/53 
f 70/607/70 75/608/75 55/609/55 
f 55/610/55 53/611/53 70/612/70 
f 85/613/85 78/614/78 75/615/75 
f 75/616/75 70/617/70 85/618/85 
f 118/619/118 107/620/107 78/621/78 
f 78/622/78 85/623/85 118/624/118 
f 143/625/143 131/626/131 107/627/107 
f 107/628/107 118/629/118 143/630/143 
f 52/631/52 53/632/53 33/633/33 
f 33/634/33 30/635/30 52/636/52 
f 65/637/65 70/638/70 53/639/53 
f 53/640/53 52/641/52 65/642/65 
f 94/643/94 85/644/85 70/645/70 
f 70/646/70 65/647/65 94/648/94 
f 117/649/117 118/650/118 85/651/85 
f 85/652/85 94/653/94 117/654/117 
f 141/655/141 143/656/143 118/657/118 
f 118/658/118 117/659/117 141/660/141 
f 45/661/45 52/662/52 30/663/30 
f 30/664/30 19/665/19 45/666/45 
f 64/667/64 65/668/65 52/669/52 
f 52/670/52 45/671/45 64/672/64 
f 90/673/90 94/674/94 65/675/65 
f 65/676/65 64/677/64 90/678/90 
f 109/679/109 117/680/117 94/681/94 
f 94/682/94 90/683/90 109/684/109 
f 134/685/134 141/686/141 117/687/117 
f 117/688/117 109/689/109 134/690/134 
f 42/691/42 45/692/45 19/693/19 
f 19/694/19 28/695/28 42/696/42 
f 58/697/58 64/698/64 45/699/45 
f 45/700/45 42/701/42 58/702/58 
f 80/703/80 90/704/90 64/705/64 
f 64/706/64 58/707/58 80/708/80 
f 99/709/99 109/710/109 90/711/90 
f 90/712/90 80/713/80 99/714/99 
f 119/715/119 134/716/134 109/717/109 
f 109/718/109 99/719/99 119/720/119 
f 46/721/46 42/722/42 28/723/28 
f 28/724/28 31/725/31 46/726/46 
f 66/727/66 58/728/58 42/729/42 
f 42/730/42 46/731/46 66/732/66 
f 77/733/77 80/734/80 58/735/58 
f 58/736/58 66/737/66 77/738/77 
f 97/739/97 99/740/99 80/741/80 
f 80/742/80 77/743/77 97/744/97 
f 116/745/116 119/746/119 99/747/99 
f 99/748/99 97/749/97 116/750/116 
# num_faces: 50

g Surface6
s 6

f 108/751/108 98/752/98 113/753/113 
f 113/754/113 125/755/125 108/756/108 
f 91/757/91 83/758/83 98/759/98 
f 98/760/98 108/761/108 91/762/91 
f 74/763/74 76/764/76 83/765/83 
f 83/766/83 91/767/91 74/768/74 
f 49/769/49 54/770/54 76/771/76 
f 76/772/76 74/773/74 49/774/49 
f 26/775/26 38/776/38 54/777/54 
f 54/778/54 49/779/49 26/780/26 
f 105/781/105 108/782/108 125/783/125 
f 125/784/125 128/785/128 105/786/105 
f 93/787/93 91/788/91 108/789/108 
f 108/790/108 105/791/105 93/792/93 
f 68/793/68 74/794/74 91/795/91 
f 91/796/91 93/797/93 68/798/68 
f 37/799/37 49/800/49 74/801/74 
f 74/802/74 68/803/68 37/804/37 
f 15/805/15 26/806/26 49/807/49 
f 49/808/49 37/809/37 15/810/15 
f 110/811/110 105/812/105 128/813/128 
f 128/814/128 124/815/124 110/816/110 
f 89/817/89 93/818/93 105/819/105 
f 105/820/105 110/821/110 89/822/89 
f 61/823/61 68/824/68 93/825/93 
f 93/826/93 89/827/89 61/828/61 
f 41/829/41 37/830/37 68/831/68 
f 68/832/68 61/833/61 41/834/41 
f 24/835/24 15/836/15 37/837/37 
f 37/838/37 41/839/41 24/840/24 
f 111/841/111 110/842/110 124/843/124 
f 124/844/124 127/845/127 111/846/111 
f 92/847/92 89/848/89 110/849/110 
f 110/850/110 111/851/111 92/852/92 
f 62/853/62 61/854/61 89/855/89 
f 89/856/89 92/857/92 62/858/62 
f 43/859/43 41/860/41 61/861/61 
f 61/862/61 62/863/62 43/864/43 
f 32/865/32 24/866/24 41/867/41 
f 41/868/41 43/869/43 32/870/32 
f 100/871/100 111/872/111 127/873/127 
f 127/874/127 112/875/112 100/876/100 
f 87/877/87 92/878/92 111/879/111 
f 111/880/111 100/881/100 87/882/87 
f 67/883/67 62/884/62 92/885/92 
f 92/886/92 87/887/87 67/888/67 
f 50/889/50 43/890/43 62/891/62 
f 62/892/62 67/893/67 50/894/50 
f 35/895/35 32/896/32 43/897/43 
f 43/898/43 50/899/50 35/900/35 
# num_faces: 50

"""

im3=pygame.Surface((128, 128))
sx,sy=im3.get_size()

textures=[]

for line in txt.split("\n"):
	if line[0:2]=="vt":
		pts=line.split(" ")
		x=float(pts[1])*sx
		y=float(pts[2])*sy
		
		textures.append((x,y))

for line in txt1.split("\n"):
	if len(line)>0 and line[0]=="f":
		pts=line.split(" ")
		t1=int(pts[1].split("/")[1])
		t2=int(pts[2].split("/")[1])
		t3=int(pts[3].split("/")[1])
		
		p1=textures[t1-1]
		p2=textures[t2-1]
		p3=textures[t3-1]
		
		pygame.draw.polygon(im3,(255,0,0),(p1,p2,p3))
	elif len(line)>0 and line[0]=="'":
		break

#m3.set_at((int(x*sx),int(y*sy)),(255,0,0))

pygame.image.save(im3,"out.bmp")