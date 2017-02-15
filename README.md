ΕΘΝΙΚΟ ΚΑΙ ΚΑΠΟΔΙΣΤΡΙΑΚΟ ΠΑΝΕΠΙΣΤΗΜΙΟ ΑΘΗΝΩΝ
ΣΧΟΛΗ ΘΕΤΙΚΩΝ ΕΠΙΣΤΗΜΩΝ
ΤΜΗΜΑ ΠΛΗΡΟΦΟΡΙΚΗΣ ΚΑΙ ΤΗΛΕΠΙΚΟΙΝΩΝΙΩΝ
ΑΝΑΠΤΥΞΗ ΛΟΓΙΣΜΙΚΟΥ ΓΙΑ ΠΛΗΡΟΦΟΡΙΑΚΑ ΣΥΣΤΗΜΑΤΑ
ΧΕΙΜΕΡΙΝΟ ΕΞΑΜΗΝΟ 2016-2017


Ονόματα  :							        Αριθμοί Μητρώου :	


Αναστάσιος Κυριακίδης 					1115201200078

Δημήτριος Αποστολόπουλος				1115201200007

Ηλίας	Κορομπίλης						    1115201200070


Η εφαρμογή αναπτύχθηκε σε γλώσσα C.


Μεταγλώτιση εφαρμογής με χρήση Makefile.

Εκτέλεση εφαρμογής μέσω της παρακάτω εντολής:
./project -i [input file] -w [workload file] -o [output file] -t [number of worker threads]
 
Εναλλακτικά :
./project –input [input file] –workload  [workload file] –output [output file] --threads [number of worker threads].

Σημείωση: Τα ορίσματα της γραμμής εντολών μπορούν να δοθούν σε οποιαδήποτε σειρά.



1. Σχόλια επί της υλοποίησης μας – σχεδιαστικές επιλογές :

Έχουμε ορίσει την δομή graph που αναπαριστά τον γράφο.
Περιέχει δύο δομές buffer και δυο δομές index (μια για τις εισερχόμενες και μια για τις εξερχόμενες ακμές αντίστοιχα), μια δομή για τα Connected Components, μια δομή για τα Strongly Connected Components.

Συγκεκριμένα: 

Για τα Connected Components: 
Η δομή ακολουθεί το πρότυπο της εκφώνησης, με τις εξής προσθήκες. Αντί για “updateIndex” (με την έννοια που παρουσιάζεται στην εκφώνηση και εξηγήθηκε στα φροντιστήρια), επιλέξαμε να αποθηκευούμε τις συνδέσεις των components (που προκύπτουν από additions μεταξύ αυτών) σε μια δομή γράφου αντίστοιχη με την περιγραφόμενη παραπάνω (HyperGraph , δηλαδή , για τα Connected Components). Επομένως, όταν ελέγχουμε για τη σύνδεση δύο components, πρακτικά εκτελούμε Bidirectional BFS στον προαναφερθέντα HyperGraph. Σε περίπτωση, δε, που απαιτείται rebuild, αφότου ανανεωθεί η πληροφορία για τα components (σε ποιό , δηλαδή , component ανήκει ο κάθε κόμβος του γράφου), ο παραπάνω HyperGraph διαγράφεται και δημιουργείται εκ νέου.

Αναφορικά στο metric value, υπολογίζουμε το πηλίκο των Bid-BFS που πραγματοποιήθηκαν στον παραπάνω HyperGraph και βρήκαν μονοπάτι μεταξύ δύο Components προς το συνολικό αριθμό Bid-BFS που πραγματοποιήθηκαν στον HyperGraph (επιτυχών και μη). Στο τέλος κάθε ριπής, το main thread εκτελεί rebuild, σε περίπτωση που το εν λόγω πηλίκο υπερβαίνει την defined (στο header.h) τιμή METRIC_VAL. 
Παρατηρήσαμε πως χρονικά δε συμφέρει συχνό rebuild, για αυτό και η METRIC_VAL είναι ίση με 1. Προφανώς, αν μειωθεί αρκετά θα προκληθούν rebuilds κατά την εκτέλεση της εφαρμογής.

Για τα Strongly Connected Components: 
Υλοποιήθηκε επαναληπτική μορφή του αλγορίθμου του Tarjan, για τη δημιουργία των Strongly Connected Components και για αυτό έχει ορισθεί και χρησιμοποιηθεί  η δομή Node (σχόλια επί της οποίας βρίσκονται στα αρχεία του κώδικα SCC.h). 
Για την υλοποιήση των πολλαπλών Grail ευρετηρίων κάνουμε πολλαπλές διασχίσεις του HyperGraph τοποθετώντας κατάλληλα labels στους κόμβους του. Πριν από την κάθε διάσχιση, με σκοπό την τυχαία προσπέλαση των κόμβων αυτών, ανακατεύουμε με τυχαίο τρόπο τα ids τους (χρησιμοποιείται η συνάρτηση ShuffleArray). Για περισσότερες λεπτομέρειες, παραπέμπουμε στα σχόλια του κώδικα μας. 


Ο έλεγχος για αποφυγή διπλοτύπων ακμών γράφου γίνεται χρησιμοποιώντας επεκτατό κατακερματισμό. Κρίναμε ότι κάτι τέτοιο δεν είναι αναγκαίο να πραγματοποιηθεί στον HyperGraph των Connected Components για να μην σπαταληθεί περισσότερος χώρος  και χρόνος (για αυτό και πραγματοποιείται γραμμικός έλεγχος για ύπαρξη ακμών) .



2.   Χρόνοι εκτέλεσης εφαρμογής για κάθε dataset :

medium datasets:

[Δεν παρατηρήσαμε αισθητή διαφορά πριν και μετα την υποστήριξη πολλαπλών νημάτων εκτέλεσης, καθώς το workload είναι σχετικά μικρό ωστε να εκτελείται γρήγορα και από έναν μόνο worker).]

Οι τελικοί χρόνοι εκτέλεσης με δύο worker που είναι ο βέλτιστος αριθμός, και ένα ευρετήτιο  grail στην περίπτωση του στατικού (αλλαγή στην define τιμή στο header.h) είναι οι παρακάτω:

Dynamic workload:    περίπου 2.5 sec 

Static workload:     περίπου 3 sec


large datasets:

Πριν την υποστήριξη πολλαπλών νημάτων εκτέλεσης, με 5 ευρετήρια grail στην περίπτωση του στατικού,  οι χρόνοι ήταν περίπου οι εξής:

Static workload:  περίπου 1.20 min

Dynamic workload: περίπου 9 min

Μετά την προσθήκη πολλαπλών νημάτων εκτέλεσης (4 worker threads), 5 grail indexes:

Static workload:  περίπου 45 sec

Dynamic workload: περίπου 3 min


(Οι παραπάνω χρόνοι εκτέλεσης προέκυψαν από μετρήσεις σε υπολογιστή με επεξεργαστή 
Intel® Core™ i7-4700MQ CPU @ 2.40GHz)




3.   Unit testing για την εφαρμογή :

Υπάρχει αντίστοιχος φάκελος στα παραδοτέα. 

Μεταγλώττιση προγράμματος unit testing μέσω εντολής Makefile. 
Εκτέλεση με την εντολή :

./unit_test unit_test_graph.txt unit_test_workload_dynamic.txt unit_test_graph2.txt unit_test_workload_static.txt
