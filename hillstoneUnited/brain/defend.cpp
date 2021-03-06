#include "defend.hpp"

//#define TEST

Defend::Defend(World& w, double _initpos[]){
  finish_flag = false;
  beam_flag = false;

  ballpos[0] = 0.0;
  ballpos[1] = 0.0;
  bal[0] = w.getBAL(0);
  bal[1] = w.getBAL(1);
  bal[2] = w.getBAL(2);
  mypos[0] = 0.0;
  mypos[1] = 0.0;
  balposconf = w.confBAL();
  myposconf = w.confXY();
  angle = w.getABSANGLE();
  dist = 0.0;

  for(int i=0; i<11; i++){
    for(int j=0; j<4; j++){
      friends[i][j] = w.getFRIEND(i, j);
      enemy[i][j] = w.getENEMY(i, j);
    }
  }

  field_x = w.getFieldLengthX();
  field_y = w.getFieldLengthY();

  initpos[0] = _initpos[0];
  initpos[1] = _initpos[1];
  initpos[2] = _initpos[2];
  //  elementList.push_back(new SequenceMovement("LAROUND"));
  judgement(w);
}

void Defend::judgement(World& w){

  ballpos[0] = w.getBXY(0);
  ballpos[1] = w.getBXY(1);
  bal[0] = w.getBAL(0);
  bal[1] = w.getBAL(1);
  bal[2] = w.getBAL(2);
  mypos[0] = w.getXY(0);
  mypos[1] = w.getXY(1);
  balposconf = w.confBAL();
  myposconf = w.confXY();
  angle = w.getABSANGLE();
  dist = sqrt(((initpos[0] - mypos[0])*(initpos[0] - mypos[0]))
	      + ((initpos[1] - mypos[1])*(initpos[1] - mypos[1])));

  t_count = abs(15 * bal[1] / 360);
  if(t_count == 0){
    t_count = 1;
  }
  
  for(int i=0; i<11; i++){
    for(int j=0; j<4; j++){
      friends[i][j] = w.getFRIEND(i, j);
      enemy[i][j] = w.getENEMY(i, j);
    }
  }

#ifndef TEST


  if(inTerritory()){
    //    std::cout << "in my Territory." << std::endl;
    if(dist < 10.0){
      if(balposconf==300 && myposconf==300){
        elementList.push_back(new TicktackBase("TLEFT", 2));
      }
      else if(balposconf==300){
        elementList.push_back(new TicktackBase("TLEFT", t_count));
        elementList.push_back(new SequenceMovement("LAROUND"));
      }
      else{
        if(close2Bal() && towardEnemy()){
	  //          std::cout << "close to ball" << std::endl;
          if(bal[0] > 10){
            elementList.push_back(new TicktackBase("TLEFT", t_count));
          }
          else if(bal[0] < -10){
            elementList.push_back(new TicktackBase("TRIGHT", t_count));
          }
          else{
            elementList.push_back(new SequenceMovement("DUMMY"));
            elementList.push_back(new TicktackBase("FORWARD", 2));
            elementList.push_back(new GABase("GA_FORWARD", 5));
          }
          //	  elementList.push_back(new KickTo(w, 0));
        }else{
          if(getInvader()>0){
	    //            std::cout << "invader, invader!" << std::endl;
            elementList.push_back(new RunToEnemy(w, getInvader()));
          }
          else{
            if(bal[0] < 2.0){
	      //              std::cout << "chuuto hanpa" << std::endl;
            // elementList.push_back(new RunTo(w, ballpos));
              if(bal[1] < -15){
                elementList.push_back(new TicktackBase("TRIGHT", t_count));
              }
              else if(bal[1] > 15){
                elementList.push_back(new TicktackBase("TLEFT", t_count));
              }
              else{
		//		elementList.push_back(new SequenceMovement("DUMMY"));
		//                elementList.push_back(new TicktackBase("DRIBBLE", 5));
		elementList.push_back(new OdensWalk(ballpos));
              }
            }else{
	      //              std::cout << "not close to ball" << std::endl;
              elementList.push_back(new RunToBall(w));
            }
          }
        }
      }
    }
    else{
      //      std::cout << "to far, I'll back" << std::endl;
      if(!close2Bal()){
        double home[2];
        for(int i=0; i<2; i++){
          home[i] = initpos[i];
        }
        elementList.push_back(new RunTo(w, home));
      }
    }
  }else{
    //    std::cout << "not in my Territory." << std::endl;
    if(balposconf==300 && myposconf==300){
      elementList.push_back(new SequenceMovement("DUMMY"));
      elementList.push_back(new SequenceMovement("LAROUND"));
      elementList.push_back(new TicktackBase("TLEFT", 2));
    }
    else{
      if(atHome()){
        if(balposconf==300 && myposconf==300){
          elementList.push_back(new TicktackBase("TLEFT", 2));
        }
        else if(myposconf==300){
          elementList.push_back(new SequenceMovement("DUMMY"));
          elementList.push_back(new SequenceMovement("LAROUND"));
        }
        else{
          if(bal[1] > 0){
            elementList.push_back(new TicktackBase("TLEFT", t_count));
          }
          else{
            elementList.push_back(new TicktackBase("TRIGHT", t_count));
          }
        }
      }
      else{
        double home[2];
        for(int i=0; i<2; i++){
          home[i] = initpos[i];
        }
        elementList.push_back(new OdensWalk(home));
      }
    }
  }
  // elementList.push_back(new SequenceMovement("DUMMY"));
  // elementList.push_back(new SequenceMovement("LAROUND"));

#endif
#ifdef TEST
  double hoge[2] = {0.0, 0.0};
  hoge[0] = initpos[0] + 1;
  hoge[1] = initpos[1];
  elementList.push_back(new OdensWalk(hoge));
  elementList.push_back(new GABase("GA_FORWARD", 5));

#endif
}

bool Defend::inTerritory(){
  if(abs(ballpos[0] - initpos[0]) < 6.0 &&
     abs(ballpos[1] - initpos[1]) < 6.0){
    return true;
  }else{
    return false;
  }
}

bool Defend::atHome(){
  if(abs(mypos[0] - initpos[0]) < 0.5 &&
     abs(mypos[1] - initpos[1]) < 0.5){
    return true;
  }else{
    return false;
  }
}

bool Defend::close2Bal(){
  if(abs(bal[0]) < 0.9){
    return true;
  }else{
    return false;
  }
}

bool Defend::towardEnemy(){
  if(abs(angle) < 60){
    return true;
  }else{
    return false;
  }
}

bool Defend::inDanger(){
  if(ballpos[0] < -(1*field_x/3)){
    return true;
  }else{
    return false;
  }
}

void Defend::updateFinishFlag(World& w){
  // if(!inTerritory() && atHome()){
  //   finish_flag = true;
  // }else{
  //   finish_flag = false;
  // }
  judgement(w);
}

int Defend::getInvader(){
  int nearest = 0;
  double dist = 1000;
  for(int i=0; i<10; i++){
    if(abs(enemy[i][0] - bal[0]) < 3.0 &&
       abs(enemy[i][1] - bal[1]) < 30 &&
       enemy[i][3] != 0){
      if(abs(enemy[i][0] - bal[0]) < dist){
	dist = abs(enemy[i][0] - bal[0]);
	nearest = i;
      }
    }
  }
  if(nearest!=0){
    return nearest;
  }
  else{
    return 0;
  }
}

std::string Defend::getNextAngle(World& w) {
    std::stringstream ss;
    if((w.getPlaymode()=="BeforeKickOff" ||
       w.getPlaymode()=="Goal_Left" ||
       w.getPlaymode()=="Goal_Right") && w.getUnum()>0){
        beam_flag = true;
        ss << "(beam " << initpos[0] << " "
                << initpos[1] << " " << initpos[2]
                << ")";
        // std::cout << ss.str() << std::endl;
    }
    
    if (w.isFalling())
    {
        if (pushStand)
        {
            /* code */
        } else {
            elementList.clear();
            elementList.push_front(new Standup());
            pushStand = true;
        }
    } else {
        pushStand = false;
    }

    rtn = elementList.front()->getNextAngle(w);
    if(beam_flag){
        rtn += ss.str();
        beam_flag = false;
    }
    if (elementList.front()->isFinished())
    {
        ElementBase* tmp = elementList.front();
        delete tmp;
        elementList.pop_front();
    }
    if (finishAllChild(w))
    {
        updateFinishFlag(w);
    }
    return rtn;
}

