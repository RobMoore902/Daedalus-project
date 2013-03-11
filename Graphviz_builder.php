<?php

// require_once ("config/mysql.config.php");
/**
 * @file
 * daedalus_browse_graphviz.php
 * Created by Dr. Blouin, Justin Joyce and Matthew Irving
 */
/**
 * Menu Location: Daedalus -> Browse -> Graphviz
 * URL Location:  daedalus/browse/graphviz
 *
 * Displays visual course, slo and program maps.
 */
 
//example on how to generate graph.
//accepts input of course name
// $link = graphviz("CSCI 2121");
// echo '<img src="' . $link . '">';

$con = $mysql_link;

//echo $link;
//echo "graphviz/constellation34.png";

function graphviz($course_num) {
    //include connection
  require("config/mysql.config.php");
  $con = $mysql_link;

    // Get the working directory and path
    // for the dot and neato programs.
    $pwd = exec('pwd');
    //$graphviz_path = $page_settings['graphviz path'];


    $output = "digraph Constellation {\n";
    $output .= "size=\"10,13\"\n";

    // Get all of the course information
    $result = mysqli_query($con, "SELECT * FROM dae_course WHERE course='" . $course_num . "'");
    while ($row = mysqli_fetch_assoc($result)) {
      $selected_id = $row['id'];
        $course = $row['course'];
        $course_code = $row['course_code'];
        $course_number = $row['course_number'];
        $course_name = $row['course_name'];
    }

    // And the selected course to the map
    $output .= " course" . $selected_id . " [shape=box, style=\"rounded,filled\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=9];\n";

    $preq_courses = array();

    // Get the prerequisite course information for the selected course
    $result = mysqli_query($con, "SELECT prereq_id, set_id FROM dae_prereq_course WHERE course_id=\"" . $selected_id . "\" ORDER BY prereq_id, course_id");
    while ($row = mysqli_fetch_assoc($result)) {
        $preq_courses[$row['prereq_id']] = $row['set_id'];
    }    

    if ($preq_courses) {

        // Create the selected course prerequisite courses
        foreach ($preq_courses as $pid => $set_id) {

            $result = mysqli_query($con, "SELECT * FROM dae_course WHERE id='" . $pid . "'");
            while ($row = mysqli_fetch_assoc($result)) {

                $course = $row['course'];
                $course_code = $row['course_code'];
                $course_number = $row['course_number'];
                $course_name = $row['course_name'];

                // Add the selected course to the map.
                $output .= " course" . $pid . " [shape=box, style=\"rounded\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=7, height=0.4, width=0.6];\n";

                if ($set_id > 1) {
                    $edges .= " course" . $pid . " -> course" . $selected_id . " [style=\"setlinewidth(2), dashed\"];\n";
                } else {
                    $edges .= " course" . $pid . " -> course" . $selected_id . " [style=\"setlinewidth(2)\"];\n";
                }
            }

            // Select the prerequiste courses to the prerequisite course
            $prepreq_courses = array();

            $result = mysqli_query($con, "SELECT prereq_id, set_id FROM dae_prereq_course WHERE course_id='" . $pid . "' ORDER BY prereq_id, course_id");
            while ($row = mysqli_fetch_assoc($result)) {
                $prepreq_courses[$row['prereq_id']] = $row['set_id'];
            }

            // Create the selected course's pre prerequisite courses
            if ($prepreq_courses) {

                foreach ($prepreq_courses as $preid => $set_id) {

                    $result = mysqli_query($con, "SELECT * FROM dae_course WHERE id='" . $preid . "'");
                    while ($row = mysqli_fetch_assoc($result)) {
                        $course = $row['course'];
                        $course_code = $row['course_code'];
                        $course_number = $row['course_number'];
                        $course_name = $row['course_name'];

                        // And the selected course to the map
                        $output .= " course" . $preid . " [shape=box, style=\"rounded\", label=\"" . $course . "\", URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\", fontsize=5, height=0.35, width=0.5];\n";

                        if ($set_id > 1) {
                            $edges .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(2), dashed\"];\n";
                        } else {
                            $edges .= " course" . $preid . " -> course" . $pid . " [style=\"setlinewidth(1)\"];\n";
                        }
                    }
                }
            }
        }
    }

    $post_courses = array();

    // Get all of the post-requisite course information
    $result = mysqli_query($con, "SELECT course_id, set_id FROM dae_prereq_course WHERE prereq_id='" . $selected_id . "' ORDER BY prereq_id, course_id");
    while ($row = mysqli_fetch_assoc($result)) {
        $post_courses[$row['course_id']] = $row['set_id'];
    }

    if ($post_courses) {

        foreach ($post_courses as $postid => $set_id) {

            $result = mysqli_query($con, "SELECT * FROM dae_course WHERE id='" . $postid . "'");
            while ($row = mysqli_fetch_assoc($result)) {
                $course = $row['course'];
                $course_code = $row['course_code'];
                $course_number = $row['course_number'];
                $course_name = $row['course_name'];

                // And the selected course to the map.
                $output .= " course" . $postid . " [shape=box, style=\"rounded\", label=\"" . $course . "\" URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\" fontsize=7, height=0.4, width=0.6];\n";

                if ($set_id > 1) {
                    $edges .= " course" . $selected_id . " -> course" . $postid . " [style=\"setlinewidth(2), dashed\"];\n";
                } else {
                    $edges .= " course" . $selected_id . " -> course" . $postid . " [style=\"setlinewidth(2)\"];\n";
                }
            }

            // Select the prerequiste courses to the prerequisite course
            $postpost_courses = array();

            $result = mysqli_query($con, "SELECT course_id, set_id FROM dae_prereq_course WHERE prereq_id='" . $postid . "' ORDER BY prereq_id, course_id");
            while ($row = mysqli_fetch_assoc($result)) {
                $postpost_courses[$row['course_id']] = $row['set_id'];
            }

            if ($postpost_courses) {

                foreach ($postpost_courses as $post_postid => $set_id) {

                    $result = mysqli_query($con, "SELECT * FROM dae_course WHERE id='" . $post_postid . "'");
                    while ($row = mysqli_fetch_assoc($result)) {
                        $course = $row['course'];
                        $course_code = $row['course_code'];
                        $course_number = $row['course_number'];
                        $course_name = $row['course_name'];

                        // And the selected course to the map
                        $output .= " course" . $post_postid . " [shape=box, style=\"rounded\", label=\"" . $course . "\" URL=\"" . $course_url . "/" . $course_code . "/" . $course_number . "\" fontsize=5, height=0.35, width=0.5];\n";

                        if ($set_id > 1) {
                            $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(2), dashed\"];\n";
                        } else {
                            $edges .= " course" . $postid . " -> course" . $post_postid . " [style=\"setlinewidth(1)\"];\n";
                        }
                    }
                }
            }
        }
    }

    $output .= $edges;
    $output .= '}';


    $constellation = 'constellation';

    // Remove old constellation map filesl
    system('rm ' . $pwd . '/graphviz/constellation*');

    // Write the .svg and .png output to file
    $my_file = $pwd . '/graphviz/' . $constellation . '.dot';
    $file_handle = fopen($my_file, 'w+') or die("can't open filesss");
    fwrite($file_handle, $output);
    fclose($file_handle);

    // Execute Graphviz to create the svg output.
    system('/usr/bin/dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tsvg -o ' . $pwd . '/graphviz/' . $constellation . '.svg');

    // Execute Graphviz to create the png output.
    system('/usr/bin/dot ' . $pwd . '/graphviz/' . $constellation . '.dot -Tpng -o ' . $pwd . '/graphviz/' . $constellation . '.png');
    
    chmod('graphviz/constellation.png', 0755);
    
    //return the link
    return 'graphviz/' . $constellation . '.png';
}

?>