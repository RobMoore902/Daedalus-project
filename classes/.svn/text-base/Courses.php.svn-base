<?php

require_once('Query.php');

class Courses {
  
  public static function getCourse($course_id) {
    $course_obj = Query::d_queryRow('dae_course', array('id'=>$course_id), null);
    
    return $course_obj;
  }
  
  //returns an array of course objects that are prereq of specified course id
  public static function getPrereq($course_id) {
    $courses_array = Query::d_queryAll('dae_prereq_course', array('course_id'=>$course_id), null);
    
    $prereq_array = array();
    foreach($courses_array as $obj) {
      $course_obj = Query::d_queryRow('dae_course', array('id'=>$obj->prereq_id), null);
      $prereq_array[$course_obj->id] = $course_obj;
    }
    
    $sort_array = array();
    foreach($prereq_array as $obj) {
      $sort_array[$obj->course_number] = $obj;
    }
    ksort($sort_array);
    $prereq_array = $sort_array;
    
    return $prereq_array;
  }
  
  //returns an of course objects that are postreq of specified course id
  public static function getPostreq($course_id) {
    $courses_array = Query::d_queryAll('dae_prereq_course', array('prereq_id'=>$course_id), null);
    
    $postreq_array = array();
    foreach($courses_array as $obj) {
      $course_obj = Query::d_queryRow('dae_course', array('id'=>$obj->course_id), null);
      $postreq_array[$course_obj->id] = $course_obj;
    }
    
    $sort_array = array();
    foreach($postreq_array as $obj) {
      $sort_array[$obj->course_number] = $obj;
    }
    ksort($sort_array);
    $postreq_array = $sort_array;
    
    return $postreq_array;
  }
  
  //returns an array of course objects given an slo_id
  public static function getCoursesBySLO($slo_id) {
    $courses_array = Query::d_queryAll('dae_course_slo', array('slo_id'=>$slo_id), null);
    
    $courses_list = array();
    foreach($courses_array as $obj) {
      if(!$courses_list[$obj->course_id]) {
        $course_obj = Query::d_queryRow('dae_course', array('id'=>$obj->course_id), null);
        $courses_list[$course_obj->id] = $course_obj;
      }
    }
    
    return $courses_list;
  }

  //returns array of course codes ie. CSCI, INFX, MATH, PREU, STAT
  public static function getCourseCodeList() {
    return Query::d_queryCol('dae_course', 'course_code');
  }

  public static function getCoursesByCode($courseCode) {
    // $courseCode_array = self::getCourseCodeList();
    $courseCode = strtoupper($courseCode);
    $courses_array = array();

    $courses_array = Query::d_queryAll('dae_course', array('course_code'=>$courseCode), null);
    $aliasCourses_array = Query::d_queryAll('dae_course_alias', array('alias_code'=>$courseCode), null);
    
    foreach($aliasCourses_array as $obj) {
      $course_obj = Query::d_queryRow('dae_course', array('id'=>$obj->parent_id), null);
      $course_obj->course_code = $obj->alias_code;
      $course_obj->course_number = $obj->alias_number;
      $course_obj->course = $obj->alias_course;
      $courses_array[] = $course_obj;
    }

    
    return $courses_array;
  }
  
  //returns array of array of course obj with hyperlink
  public static function getBrowseCourseList() {
    $courseCode_array = self::getCourseCodeList();
    
    $courses_list = array();
    foreach($courseCode_array as $code) {
      $courses_array = Query::d_queryAll('dae_course', array('course_code'=>$code), 'course_number');
      
      foreach($courses_array as $course_obj) {
        $course_obj->hyperlink = '<a href="index.php?q=course/'.strtolower($course_obj->course_code).'/'.$course_obj->course_number .'">'. $course_obj->course .' - '. $course_obj->course_name .'</a>';
        $courses_list[$code][$course_obj->course_number] = $course_obj;
      }
    }
    
    return $courses_list;
  }
  
  //returns boolean if course exists given code and number
  public static function courseExists($courseCode, $courseNumber) {
    $course_obj = Query::d_queryRow('dae_course', array('course_code'=>$courseCode, 'course_number'=>$courseNumber));
    if($course_obj) {
      return true;
    }
    
    return false;
  }
  
}

















?>

