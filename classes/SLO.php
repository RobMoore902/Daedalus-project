<?php

require_once('Query.php');
require_once('Core.php');

class SLO {
  
  public static function getSLO($slo_id) {
    $slo_obj = Query::d_queryRow('dae_slo', array('id'=>$slo_id), null);
    
    return $slo_obj;
  }
  
	public static function getSLOPostReq($slo_id) {
		$SLO_id_array = Query::d_queryCol('dae_prereq_slo', 'target', array('pre_slo'=>$slo_id));
		
		$slo_array = array();
		foreach($SLO_id_array as $id) {
			$obj = self::getSLO($id);
			$slo_array[$obj->id] = $obj->slo_text;
		}
		
		asort($slo_array);
		
		return $slo_array;
	}
	
  //returns an array of student learning outcome objects given an course_id
  public static function getSLObyCourse($course_id) {
    $slo_array = Query::d_queryAll('dae_course_slo', array('course_id'=>$course_id), null);
    
		if($slo_array) {
			$slo_list = array();
			foreach($slo_array as $obj) {
				if(!$slo_list[$obj->slo_id]) {
					$slo_obj = Query::d_queryRow('dae_slo', array('id'=>$obj->slo_id), null);
					$slo_list[$slo_obj->id] = $slo_obj;
				}
			}
			return $slo_list;
		}
    return null;
  }
  
  //returns an array of assumed learning outcome objects given an course_id
  public static function getALObyCourse($course_id) {
    $slo_array = self::getSLObyCourse($course_id);
    // return $slo_array;
    $alo_list = array();
    
    foreach($slo_array as $obj) {
      $alo_array = Query::d_queryAll('dae_prereq_slo', array('target'=>$obj->id), null);
      if($alo_array) {
        foreach($alo_array as $alo_obj) {
          if(!$alo_list[$alo_obj->pre_slo]) {
            $alo_list[$alo_obj->pre_slo] = Query::d_queryRow('dae_slo', array('id'=>$alo_obj->pre_slo), null);;
          }
        }
      }
    }
    
    $temp_array = $alo_list;
    $alo_list = array();
    foreach($temp_array as $key=>$obj) {
      if(!$slo_array[$key]) {
        $alo_list[$key] = $obj;
      }
    }
    
    return $alo_list;
  }
  
	//returns array of Array of strings for Assumed Learning Outcomes and Learning Outcomes
  public static function getCourseLOList($course_id) {
		
		$alo_list = self::getALObyCourse($course_id);
		$alo_string = '';
		foreach($alo_list as $obj) {
			if($obj->id) {
			$alo_string .= '<i class="icon-chevron-right"></i>
				<a href="index.php?q=slo/'.$obj->id .'">'.$obj->slo_text .'</a>
				<br />';
			}
		}
		
		$slo_list = self::getSLObyCourse($course_id);
		$slo_string = '';
		foreach($slo_list as $obj) {
			if($obj->id) {
			$slo_string .= '<i class="icon-chevron-right"></i>
				<a href="index.php?q=slo/'.$obj->id .'">'.$obj->slo_text .'</a>
				<br />';
			}
		}
		
		return array('alo'=>$alo_string, 'slo'=>$slo_string);
  }
  
	//returns array of key = alphabet val=string of hyperlinked slo
	public static function getSLOAlphaOrdering() {
		$allSLO_array = Query::d_queryAll('dae_slo', null, 'slo_text ASC');
		
		$alphaSLO_array = array();
		
		foreach($allSLO_array as $obj) {
			$alphaSLO_array['all'] .= '<i class="icon-chevron-right"></i>
				<a href="index.php?q=slo/'.$obj->id .'">'.$obj->slo_text .'</a>
				<br />';
				$string = $obj->slo_text;
				$firstLetter = $string[0];
				$alphaSLO_array[strtoupper($firstLetter)] .= '<i class="icon-chevron-right"></i>
					<a href="index.php?q=slo/'.$obj->id .'">'.$obj->slo_text .'</a>
					<br />';
		}
		
		return $alphaSLO_array;
	}
	
}

