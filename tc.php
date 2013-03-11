<?php

	function tc($con , $paramTags) {
		$result = mysqli_query($con, "
			SELECT setting, value
			FROM dae_settings
			WHERE setting='browse learning outcomes' OR
			setting='manage learning outcomes' OR
			setting='goal identification' OR
			setting='question mark' OR
			setting='tag cloud max font size' OR
			setting='build learning outcomes' OR
			setting='tag cloud height percent' OR
			setting='show tag cloud settings'");

		$page_settings = array();
		while ($row = mysqli_fetch_assoc($result)) {
			$page_settings[$row['setting']] = $row['value'];
		}

		global $base_url;
		global $user;

		//create URL
		$page_url = $_SERVER['PHP_SELF'] . "?";
		$help_url = $_SERVER['PHP_SELF'] . "?";
		$page_settings['browse learning outcomes'] = $_SERVER['PHP_SELF'] . "?";

		// The Manage SLO URL.
		$slo_url = $page_settings['manage learning outcomes'];

		// URL Parameters.
		$param = array();
		$param[0] = 'selected';
		$param[1] = $paramTags;

		$slo_id = $param[0];
		$tags = $param[1];
		
		// Make sure there are SLOs present.
		if (!mysqli_num_rows(mysqli_query($con, "SELECT * FROM dae_slo"))) {
			if ($build_access) {

				// If there are no learning outcomes, inform
				// the user a SLO must be created first.s
				echo "There are no learning outcomes created.";
			}
		}
		else {
			// Initialize the cloud tag string.
			$cloud_string = '';

			// If a tag is not selected display the entire tag cloud.
			if ($slo_id != 'selected') {

				$tag_array = array();

				$min = -1;
				$max = -1;
				$i = 0;

				// Create the entire tag array.
				$result = mysqli_query($con, "
					SELECT *
					FROM dae_tag
					ORDER BY tag_label");
				while ($row = mysqli_fetch_assoc($result)) {

					$tag_count = mysqli_num_rows(mysqli_query($con, "
						SELECT *
						FROM dae_slo_tag
						WHERE tag_id=\"" . $row['id'] . "\""));

					if ($tag_count) {
						$tag_array[$i]['tag_id'] = $row['id'];
						$tag_array[$i]['label'] = $row['tag_label'];
						$tag_array[$i]['count'] = $tag_count;
						$i++;

						// Store the min and max tag counts to
						// help calculate the tag cloud text size.
						if ($min == -1) {
								$min = $max = $tag_count;
						} else if ($tag_count < $min && $tag_count != 0) {
								$min = $tag_count;
						} else if ($tag_count > $max) {
								$max = $tag_count;
						}
					}
				}
			}
			else {

					// Else a tag has been selected so narrow the tags in the
					// cloud tag accoring to the select tag. tag tag tag

					$label_index = array();

					// Make a list of ids and their tag labels to reduce
					// the amount of database access required for the script.
					$result = mysqli_query($con, "
						SELECT *
						FROM dae_tag
						ORDER BY tag_label");

					while ($row = mysqli_fetch_assoc($result)) {
							$label_index[$row['id']] = $row['tag_label'];
					}

					// If the tags are not delimited by an underscore
					// select the results with only the singel tag.
					if (!strrpos($tags, '_')) {
							// Deselect all tags by clicking the only selected tag.
							$selected_tags = '<a href="' . $page_url . '">#' . $label_index[$tags] . '</a>';

							$slo_array = array();

							// Get all the slo id's for the selected tag.
							$result = mysqli_query($con, "
								SELECT slo_id
								FROM dae_slo_tag
								WHERE tag_id=\"" . $tags . "\"
								ORDER BY slo_id");

							while ($row = mysqli_fetch_assoc($result)) {
									$slo_array[] = $row['slo_id'];
							}

							// Create the query string placeholder to select the slos.		
							//$slo_placeholders = implode(' OR ', array_fill(0, count($slo_array), 'slo_id=%d'));
							$slo_placeholders = or_select_statment('id', $slo_array);

							$result_tag = array();

							$result = mysqli_query($con, "
								SELECT DISTINCT tag_id
								FROM dae_slo_tag
								WHERE " . $slo_placeholders . "
								ORDER BY tag_id");
							while ($row = mysqli_fetch_assoc($result)) {

									if ($row['tag_id'] != $tags) {
											$result_tag[] = $row['tag_id'];
									}
							}

							if ($result_tag) {

									$ordered_tags = array();

									// Create the query string placeholder to select the tags again in alphabetical order
									$tag_placeholders = or_select_statment('id', $result_tag);

									$result = mysqli_query($con, "
										SELECT id
										FROM dae_tag
										WHERE " . $tag_placeholders . "
										ORDER BY tag_label");
									while ($row = mysqli_fetch_assoc($result)) {
											$ordered_tags[] = $row['id'];
									}

									// Calculate the total number of SLO's associated with the the
									// current tag being computed for the tag cloud.
									foreach ($ordered_tags as $current_tag) {

											$slo_array1 = array();
											$slo_array2 = array();

											// Get the slo ids using the URL's tag
											$result = mysqli_query($con, "
												SELECT slo_id
												FROM dae_slo_tag
												WHERE tag_id=\"" . $tags . "\"");
											while ($row = mysqli_fetch_assoc($result)) {
													$slo_array1[] = $row['slo_id'];
											}

											// Get the slo ids using the current tag
											$result = mysqli_query($con, "
												SELECT slo_id
												FROM dae_slo_tag
												WHERE tag_id=\"" . $current_tag . "\"");
											while ($row = mysqli_fetch_assoc($result)) {
													$slo_array2[] = $row['slo_id'];
											}

											// Combine each lists make sure there are no duplicate
											// entries and the number of matching items is the count.
											$temp_slos = array_intersect($slo_array1, $slo_array2);
											$temp_slos = array_unique($temp_slos);
											$tag_count = count($temp_slos);

											$tag_array[$current_tag]['tag_id'] = $current_tag;
											$tag_array[$current_tag]['count'] = $tag_count;
											$tag_array[$current_tag]['label'] = $label_index[$current_tag];

											// Store the min and max tag count to
											// help calculate the tag cloud text size.
											if (!$min) {
													$min = $max = $tag_count;
											} else if ($tag_count < $min) {
													$min = $tag_count;
											} else if ($tag_count > $max) {
													$max = $tag_count;
											}
									}
							}
					}
					else {
						// Now the url parameter has more than one tag
						// selected. Turn the tags into an array of tags.
						$tags = explode('_', $tags);

						// Create the selected tag string with a twist. When two tags or more
						// have been selected, the selected tags string now has the option of
						// deselecting one of the tags or all of them.
						if (count($tags) == 2) {

							$result = mysqli_query($con, "
								SELECT slo_id
								FROM dae_slo_tag
								WHERE tag_id=\"" . $tags[0] . "\" OR tag_id=\"" . $tags[1] . "\"
								ORDER BY slo_id");
							while ($row = mysqli_fetch_assoc($result)) {
								$slo_array[] = $row['slo_id'];
							}

							$slo_array = array_unique($slo_array);

							$slo_placeholders = or_select_statment('slo_id', $slo_array);

							$result = mysqli_query($con, "
								SELECT DISTINCT tag_id
								FROM dae_slo_tag
								WHERE " . $slo_placeholders . "
								ORDER BY tag_id");
							while ($row = mysqli_fetch_assoc($result)) {

								// Make sure not to add the selected tags.
								if (!in_array($row['tag_id'], $tags)) {
									$result_tag[] = $row['tag_id'];
								}
							}

							if ($result_tag) {

								$tag_placeholders = or_select_statment('id', $result_tag);

								$result = mysqli_query($con, "
									SELECT id FROM dae_tag
									WHERE " . $tag_placeholders . "
									ORDER BY tag_label");
								while ($row = mysqli_fetch_assoc($result)) {
									$ordered_tags[] = $row['id'];
								}

								foreach ($ordered_tags as $current_tag) {

									$slo_array1 = array();
									$slo_array2 = array();
									$slo_array3 = array();

									// Calculate the total number of
									// SLO's associated with Tag 1.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $tags[0] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
											$slo_array1[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 2.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $tags[1] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
											$slo_array2[] = $row['slo_id'];
									}

									// Calculate the total number of SLOs
									// associated with the current tag.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $current_tag . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
											$slo_array3[] = $row['slo_id'];
									}

									// Combine each lists make sure there are no duplicate
									// entries and the number of matching items is the count.
									$temp_slos = array_intersect($slo_array1, $slo_array2);
									$temp_slos = array_intersect($slo_array3, $temp_slos);
									$temp_slos = array_unique($temp_slos);
									$tag_count = count($temp_slos);

									if ($tag_count > 0) {

											$tag_array[$current_tag]['tag_id'] = $current_tag;
											$tag_array[$current_tag]['count'] = $tag_count;
											$tag_array[$current_tag]['label'] = $label_index[$current_tag];

											// Store the min and max tag count to
											// help calculate the tag cloud text size.
											if (!$min) {
												$min = $max = $tag_count;
											}
											else if ($tag_count < $min) {
												$min = $tag_count;
											}
											else if ($tag_count > $max) {
												$max = $tag_count;
											}
									}
								}
							}
						}
						else if (count($tags) == 3) {
							// Select associated SLOs.
							$result = mysqli_query($con, "
								SELECT slo_id
								FROM dae_slo_tag
								WHERE tag_id=\"" . $tags[0] . "\" OR tag_id=\"" . $tags[1] . "\" OR tag_id=\"" . $tags[2] . "\"
								ORDER BY slo_id");

							while ($row = mysqli_fetch_assoc($result)) {
									$slo_array[] = $row['slo_id'];
							}

							// Remove duplicate values.
							$slo_array = array_unique($slo_array);

							// Create the query string placeholder to select the slos.
							$slo_placeholders = or_select_statment('slo_id', $slo_array);

							$result = mysqli_query($con, "SELECT DISTINCT tag_id FROM dae_slo_tag WHERE " . $slo_placeholders . " ORDER BY tag_id");
							while ($row = mysqli_fetch_assoc($result)) {

									// Make sure not to add the selected tags.
									if (!in_array($row['tag_id'], $tags)) {
											$result_tag[] = $row['tag_id'];
									}
							}
							
							if ($result_tag) {

								// Create the query string placeholder to select the tags again in alphabetical order.
								$tag_placeholders = or_select_statment('id', $result_tag);

								$result = mysqli_query($con, "SELECT id FROM dae_tag WHERE " . $tag_placeholders . " ORDER BY tag_label");
								while ($row = mysqli_fetch_assoc($result)) {
									$ordered_tags[] = $row['id'];
								}

								foreach ($ordered_tags as $current_tag) {

									$slo_array1 = array();
									$slo_array2 = array();
									$slo_array3 = array();
									$slo_array4 = array();

									// Calculate the total number of
									// SLO's associated with Tag 1.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $tags[0] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array1[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 2.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $tags[1] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array2[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 3.
									$result = mysqli_query($con, "
										SELECT slo_id
										FROM dae_slo_tag
										WHERE tag_id=\"" . $tags[2] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array3[] = $row['slo_id'];
									}

									// Calculate the total number of SLOs
									// associated with the current tag.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=\"" . $current_tag . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array4[] = $row['slo_id'];
									}

									// Combine each lists make sure there are no duplicate
									// entries and the number of matching items is the count.
									$temp_slos = array_intersect($slo_array1, $slo_array2);
									$temp_slos = array_intersect($slo_array3, $temp_slos);
									$temp_slos = array_intersect($slo_array4, $temp_slos);
									$temp_slos = array_unique($temp_slos);
									$tag_count = count($temp_slos);

									if ($tag_count > 0) {
										$tag_array[$current_tag]['tag_id'] = $current_tag;
										$tag_array[$current_tag]['count'] = $tag_count;
										$tag_array[$current_tag]['label'] = $label_index[$current_tag];
									}
								}
							}
						}
						else if (count($tags) == 4) {
							// Select associated SLOs.
							$result = mysqli_query($con, "
								SELECT slo_id
								FROM dae_slo_tag
								WHERE tag_id=\"" . $tags[0] . "\" OR tag_id=\"" . $tags[1] . "\" OR tag_id=\"" . $tags[2] . "\" OR tag_id=\"" . $tags[3] . "\"
								ORDER BY slo_id");

							while ($row = mysqli_fetch_assoc($result)) {
								$slo_array[] = $row['slo_id'];
							}

							$slo_array = array_unique($slo_array);

							$slo_placeholders = or_select_statment('slo_id', $slo_array);

							$result = mysqli_query($con, "
								SELECT DISTINCT tag_id
								FROM dae_slo_tag
								WHERE ". $slo_placeholders ."
								ORDER BY tag_id");
								
							while ($row = mysqli_fetch_assoc($result)) {
								// Make sure not to add the selected tags.
								if (!in_array($row['tag_id'], $tags)) {
									$result_tag[] = $row['tag_id'];
								}
							}

							if ($result_tag) {
								$tag_placeholders = or_select_statment('id', $result_tag);

								$result = mysqli_query($con, "SELECT id FROM dae_tag WHERE " . $tag_placeholders . " ORDER BY tag_label", $result_tag);
								while ($row = mysqli_fetch_assoc($result)) {
									$ordered_tags[] = $row['id'];
								}

								foreach ($ordered_tags as $current_tag) {

									$slo_array1 = array();
									$slo_array2 = array();
									$slo_array3 = array();
									$slo_array4 = array();
									$slo_array5 = array();

									// Calculate the total number of
									// SLO's associated with Tag 1.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=%d", $tags[0]);
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array1[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 2.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=\"" . $tags[1] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array2[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 3.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=\"" . $tags[2] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array3[] = $row['slo_id'];
									}

									// Calculate the total number of
									// SLO's associated with Tag 4.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=\"" . $tags[3] . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array4[] = $row['slo_id'];
									}

									// Calculate the total number of SLO's
									// associated with the current tag.
									$result = mysqli_query($con, "SELECT slo_id FROM dae_slo_tag WHERE tag_id=\"" . $current_tag . "\"");
									while ($row = mysqli_fetch_assoc($result)) {
										$slo_array5[] = $row['slo_id'];
									}

									// Combine each lists make sure there are no duplicate
									// entries and the number of matching items is the count.
									$temp_slos = array_intersect($slo_array1, $slo_array2);
									$temp_slos = array_intersect($slo_array3, $temp_slos);
									$temp_slos = array_intersect($slo_array4, $temp_slos);
									$temp_slos = array_intersect($slo_array5, $temp_slos);
									$temp_slos = array_unique($temp_slos);
									$tag_count = count($temp_slos);

									if ($tag_count > 0) {
										$tag_array[$current_tag]['tag_id'] = $current_tag;
										$tag_array[$current_tag]['count'] = $tag_count;
										$tag_array[$current_tag]['label'] = $label_index[$current_tag];
									}
								}
							}
						}
					}
			}
		}
		
		return (isset($tag_array) ? $tag_array : array());
	}


	//create long or statment for SQL
	function or_select_statment($table_names, $arr) {
		$first_time =1;
			foreach ($arr as $value) {
				if ($first_time == 1) {
					$res = $table_names ."= \"" . $value . "\" ";
					$first_time = 0;
				}
				else {
					$res .= " OR ".$table_names ." = \"" . $value . "\" ";
				}
			}

		return $res;
	}

?>
