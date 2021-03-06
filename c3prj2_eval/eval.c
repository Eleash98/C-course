#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int check_suit(card_t * card,suit_t suit);
card_t * Max_card (deck_t *hand,unsigned idx,size_t n); 


int card_ptr_comp(const void * vp1, const void * vp2) {
  const card_t *   p1=*(card_t**)vp1;
  const card_t *   p2=*(card_t**)vp2;
  if (p1->value > p2->value)
    return -1;
  if (p1->value < p2->value)
    return 1;
  if (p1->value == p2->value){
    if (p1->suit > p2->suit)
      return -1;
    if (p1->suit < p2->suit)
      return 1;
    if (p1->suit == p2->suit)
      return 0;
  }
  return 0;
}

suit_t flush_suit(deck_t * hand) {
  int n_s=0,n_h=0,n_d=0,n_c=0;
  int N = hand->n_cards;
  for (int i=0;i<N;i++){
      suit_t temp = hand->cards[i]->suit;
      if (temp == SPADES)
	n_s++;
      if(temp == HEARTS)
	n_h++;
      if(temp == DIAMONDS)
	n_d++;
      if (temp == CLUBS)
	n_c++;
    }
  if (n_s>=5)
    return SPADES;
  if (n_h>=5)
    return HEARTS;
  if (n_d>=5)
    return DIAMONDS;
  if (n_c>=5)
    return CLUBS;
  return NUM_SUITS;
}

unsigned get_largest_element(unsigned * arr, size_t n) {
  unsigned M = arr[0];
  for(int i=1;i<n;i++)
    if (arr[i] > M)
      M=arr[i];
  return M;
}

size_t get_match_index(unsigned * match_counts, size_t n,unsigned n_of_akind){
  for (unsigned i = 0;i<n;i++)
    if (*(match_counts+i)==n_of_akind)
      return i;
  assert(0);
}
ssize_t  find_secondary_pair(deck_t * hand,
			     unsigned * match_counts,
			     size_t match_idx) {
  unsigned N = hand->n_cards;
  for (unsigned i=0;i<N;i++)
    if (*(match_counts+i)>1)
      if (hand->cards[i]->value != hand->cards[match_idx]->value)
	return i;
  return -1;
}

int is_straight_at(deck_t * hand, size_t index, suit_t fs) {
  size_t N=hand->n_cards;
  int count =1,acL=2;
  card_t *current_c;
  current_c=hand->cards[index];
    for (int i=index;i<N-1;i++){
      if (current_c->value - hand->cards[i+1]->value == 1){
	if (fs != NUM_SUITS){
	  if (check_suit(current_c,fs) && check_suit(hand->cards[i+1],fs)){
	      count++;
	      current_c =hand->cards[i+1];
	  }
	}
	else {
	  count++;
	  current_c=hand->cards[i+1];
	}
      }
    }
  if (count >= 5)
    return 1;
  if (current_c->value == VALUE_ACE){
    current_c=hand->cards[N-1];
    if (current_c->value != 2)
      return 0;
    for (int i =N-1;i>index;i--){
      if (current_c->value - hand->cards[i-1]->value == -1){
	if (fs != NUM_SUITS){
	  if (check_suit(current_c,fs) && check_suit(hand->cards[i-1],fs)){
	    acL++;
	    current_c = hand->cards[i-1];
	  }
	}
	else{
	  acL++;
	  current_c = hand->cards[i-1];
	}
      }
    }
  }
  if (acL >= 5)
    return -1;
  return 0;
}
/*
int is_straight_at(deck_t * hand, size_t index, suit_t fs) {
size_t N = hand->n_cards;
  if (N<=0)
    return 0;
  int count=1,A_count=2;
  if (fs != NUM_SUITS){
    for (int i=index;i<N-1;i++){
      int T=(int)(hand->cards[i]->value) -(int)(hand->cards[i+1]->value);
      if (T==1){
	if (check_suit(hand->cards[i],fs)&&check_suit(hand->cards[i+1],fs))
	  count++;
	if (count == 5)
	  break;
      }
    }
    if (hand->cards[index]->value == VALUE_ACE && count <5 )
      for (int i=index; i<N;i++){
	if (hand->cards[i]->value == 4){
	  for (int j=i;j<N-1;j++)
	    if (hand->cards[j]->value - hand->cards[j+1]->value ==1){
	      if (check_suit(hand->cards[j],fs) && check_suit(hand->cards[j+1],fs))
		A_count++;
	      if (A_count == 5)
		break;
	    }
	}
	if (A_count == 5)
	  break;
      }
  }
   if (fs == NUM_SUITS){
    for (int i=index;i<N-1;i++){
      if ((hand->cards[i]->value) - (hand->cards[i+1]->value)==1){
	count++;
	if (count == 5)
	  break;
      }
    }
    if (hand->cards[index]->value == VALUE_ACE && count <5 )
      for (int i=index; i<N;i++){
	if (hand->cards[i]->value == 4){
	  for (int j=i;j<N-1;j++)
	    if (hand->cards[j]->value - hand->cards[j+1]->value ==1){
	      	A_count++;
	      if (A_count == 5)
		break;
	    }
	}
	if (A_count == 5)
	  break;
      }
  }
    if (A_count == 5)
      return -1;
    if (A_count !=5 && count !=5)
      return 0;
    if (count == 5)
      return 1;
  return 0;
}
*/
int check_suit(card_t * card,suit_t suit){
  if (card->suit==suit)
    return 1;
  return 0;
}
void sort(card_t **P,const int S){
  for (int i=0;i<S;i++){
    for (int j=0;j<S;j++){
      if (P[j]->value < P[i]->value){
	card_t *t=P[j];
	P[j]=P[i];
	P[i]=t;
      }
    }
  }
}
hand_eval_t build_hand_from_match(deck_t * hand,unsigned n,hand_ranking_t what,size_t idx) {

  hand_eval_t ans;
  ans.ranking = what;
  for (size_t i=0  ; i<n; i++){
    ans.cards[i] = hand->cards[i+idx];
  }
  if (n<5){
    const int SIZE=hand->n_cards-n;
    card_t *P[SIZE];
    for (int i=0,j=0;i<hand->n_cards;i++)
      if (i>=idx && i<idx+n)
	continue;
      else
	P[j++]=hand->cards[i];
    sort(P,SIZE);
    for (int i=0;i+n<5;i++)
      ans.cards[i+n]=P[i];
    /*
    for (int i=n;i<5;i++){
      card_t temp;
      unsigned N=hand->n_cards;
      temp.value=hand->cards[i]->value;
      //temp = hand->cards[i];
      for (int j=i;j<N;j++){
	if (hand->cards[j]->value > temp.value){
	  ans.cards[i]=hand->cards[j];
	  //temp=hand->cards[j];
	}
      }
      temp.value=hand->cards[0]->value;
      for (int j=0;j<i;j++){
	if (hand->cards[j]->value > temp.value){
	  ans.cards[i]=hand->cards[j];
	  //temp=hand->cards[j];
	}
      }
    }
  }
	   */
  }
  return ans;
}
  
/*
card_t *Max_card(deck_t *hand, unsigned idx,){
  unsigned N=hand->n_cards;
  ans->value=hand->cards[idx]->value;
  ans->suit=hand->cards[idx]->suit;
  for (int i=idx+1;i<N-1;i++){
    if (hand->cards[idx]->value > ans->value){
      ans->value=hand->cards[i]->value;
      ans->suit=hand->cards[i]->suit;
    }
  }
  }*/
int compare_hands(deck_t * hand1, deck_t * hand2) {
  qsort((hand1->cards),hand1->n_cards,sizeof(card_t*),card_ptr_comp);
  qsort((hand2->cards),hand2->n_cards,sizeof(card_t*),card_ptr_comp);
  hand_eval_t H1,H2;
  H1=evaluate_hand(hand1);
  H2=evaluate_hand(hand2);
  if (H1.ranking != H2.ranking){
    if (H1.ranking > H2.ranking)
      return -1;
    else
      return 1;
  }
  else {
    for (int i=0;i<5;i++)
      if (H1.cards[i]->value != H2.cards[i]->value)
	return H1.cards[i]->value-H2.cards[i]->value;
    return 0;
  }
  return 0;
}



//You will write this function in Course 4.
//For now, we leave a prototype (and provide our
//implementation in eval-c4.o) so that the
//other functions we have provided can make
//use of get_match_counts.
unsigned * get_match_counts(deck_t * hand) ;

// We provide the below functions.  You do NOT need to modify them
// In fact, you should not modify them!


//This function copies a straight starting at index "ind" from deck "from".
//This copies "count" cards (typically 5).
//into the card array "to"
//if "fs" is NUM_SUITS, then suits are ignored.
//if "fs" is any other value, a straight flush (of that suit) is copied.
void copy_straight(card_t ** to, deck_t *from, size_t ind, suit_t fs, size_t count) {
  assert(fs == NUM_SUITS || from->cards[ind]->suit == fs);
  unsigned nextv = from->cards[ind]->value;
  size_t to_ind = 0;
  while (count > 0) {
    assert(ind < from->n_cards);
    assert(nextv >= 2);
    assert(to_ind <5);
    if (from->cards[ind]->value == nextv &&
	(fs == NUM_SUITS || from->cards[ind]->suit == fs)){
      to[to_ind] = from->cards[ind];
      to_ind++;
      count--;
      nextv--;
    }
    ind++;
  }
}


//This looks for a straight (or straight flush if "fs" is not NUM_SUITS)
// in "hand".  It calls the student's is_straight_at for each possible
// index to do the work of detecting the straight.
// If one is found, copy_straight is used to copy the cards into
// "ans".
int find_straight(deck_t * hand, suit_t fs, hand_eval_t * ans) {
  if (hand->n_cards < 5){
    return 0;
  }
  for(size_t i = 0; i <= hand->n_cards -5; i++) {
    int x = is_straight_at(hand, i, fs);
    if (x != 0){
      if (x < 0) { //ace low straight
	assert(hand->cards[i]->value == VALUE_ACE &&
	       (fs == NUM_SUITS || hand->cards[i]->suit == fs));
	ans->cards[4] = hand->cards[i];
	size_t cpind = i+1;
	while(hand->cards[cpind]->value != 5 ||
	      !(fs==NUM_SUITS || hand->cards[cpind]->suit ==fs)){
	  cpind++;
	  assert(cpind < hand->n_cards);
	}
	copy_straight(ans->cards, hand, cpind, fs,4) ;
      }
      else {
	copy_straight(ans->cards, hand, i, fs,5);
      }
      return 1;
    }
  }
  return 0;
}


//This function puts all the hand evaluation logic together.
//This function is longer than we generally like to make functions,
//and is thus not so great for readability :(
hand_eval_t evaluate_hand(deck_t * hand) {
  suit_t fs = flush_suit(hand);
  hand_eval_t ans;
  if (fs != NUM_SUITS) {
    if(find_straight(hand, fs, &ans)) {
      ans.ranking = STRAIGHT_FLUSH;
      return ans;
    }
  }
  unsigned * match_counts = get_match_counts(hand);
  unsigned n_of_a_kind = get_largest_element(match_counts, hand->n_cards);
  assert(n_of_a_kind <= 4);
  size_t match_idx = get_match_index(match_counts, hand->n_cards, n_of_a_kind);
  ssize_t other_pair_idx = find_secondary_pair(hand, match_counts, match_idx);
  free(match_counts);
  if (n_of_a_kind == 4) { //4 of a kind
    return build_hand_from_match(hand, 4, FOUR_OF_A_KIND, match_idx);
  }
  else if (n_of_a_kind == 3 && other_pair_idx >= 0) {     //full house
    ans = build_hand_from_match(hand, 3, FULL_HOUSE, match_idx);
    ans.cards[3] = hand->cards[other_pair_idx];
    ans.cards[4] = hand->cards[other_pair_idx+1];
    return ans;
  }
  else if(fs != NUM_SUITS) { //flush
    ans.ranking = FLUSH;
    size_t copy_idx = 0;
    for(size_t i = 0; i < hand->n_cards;i++) {
      if (hand->cards[i]->suit == fs){
	ans.cards[copy_idx] = hand->cards[i];
	copy_idx++;
	if (copy_idx >=5){
	  break;
	}
      }
    }
    return ans;
  }
  else if(find_straight(hand,NUM_SUITS, &ans)) {     //straight
    ans.ranking = STRAIGHT;
    return ans;
  }
  else if (n_of_a_kind == 3) { //3 of a kind
    return build_hand_from_match(hand, 3, THREE_OF_A_KIND, match_idx);
  }
  else if (other_pair_idx >=0) {     //two pair
    assert(n_of_a_kind ==2);
    ans = build_hand_from_match(hand, 2, TWO_PAIR, match_idx);
    ans.cards[2] = hand->cards[other_pair_idx];
    ans.cards[3] = hand->cards[other_pair_idx + 1];
    if (match_idx > 0) {
      ans.cards[4] = hand->cards[0];
    }
    else if (other_pair_idx > 2) {  //if match_idx is 0, first pair is in 01
      //if other_pair_idx > 2, then, e.g. A A K Q Q
      ans.cards[4] = hand->cards[2];
    }
    else {       //e.g., A A K K Q
      ans.cards[4] = hand->cards[4];
    }
    return ans;
  }
  else if (n_of_a_kind == 2) {
    return build_hand_from_match(hand, 2, PAIR, match_idx);
  }
  return build_hand_from_match(hand, 0, NOTHING, 0);
}
