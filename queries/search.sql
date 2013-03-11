(
	SELECT
		`dae_course`.`course`,
		`dae_course`.`course_name`,
		`dae_slo`.`slo_text`,
		`dae_tag`.`tag_label`
	FROM
		`dae_course` 
		INNER JOIN `dae_course_slo` ON `dae_course`.`id` = `dae_course_slo`.`course_id`
		INNER JOIN `dae_slo` ON `dae_slo`.`id` = `dae_course_slo`.`slo_id`
		INNER JOIN `dae_slo_tag` ON `dae_slo`.`id` = `dae_slo_tag`.`slo_id`
		INNER JOIN `dae_tag` ON `dae_slo_tag`.`tag_id` = `dae_tag`.`id`
	WHERE
		`dae_slo`.`slo_text` LIKE '%csci 2121%'
		OR `dae_course`.`course_code` LIKE '%csci 2121%'
		OR `dae_course`.`course_name` LIKE '%csci 2121%'
		OR `dae_tag`.`tag_label` LIKE '%csci 2121%'
		OR `dae_course`.`course` LIKE '%csci 2121%'
)
UNION
(
	SELECT 
#		NULL as `course_id`,
#		`dae_slo`.`id` as `slo_id`,
#		`dae_tag`.`id` as `tag_id`,
		NULL as `course`,
		NULL as `course_name`,
		`dae_slo`.`slo_text`,
		`dae_tag`.`tag_label`

	FROM
		`dae_slo` INNER JOIN `dae_slo_tag` ON `dae_slo_tag`.`slo_id` = `dae_slo`.`id`
		INNER JOIN `dae_tag` ON `dae_tag`.`id` = `dae_slo_tag`.`tag_id`
	WHERE
		(	`dae_slo`.`slo_text` LIKE '%csci 2121%'
			OR `dae_tag`.`tag_label` LIKE '%csci 2121%'	
		)
		AND `slo_id` NOT IN
		(

			SELECT
				`dae_slo`.`id`
			FROM
				`dae_course` 
				INNER JOIN `dae_course_slo` ON `dae_course`.`id` = `dae_course_slo`.`course_id`
				INNER JOIN `dae_slo` ON `dae_slo`.`id` = `dae_course_slo`.`slo_id`
			WHERE
				`dae_slo`.`slo_text` LIKE '%csci 2121%'
				OR `dae_course`.`course_code` LIKE '%csci 2121%'
				OR `dae_course`.`course_name` LIKE '%csci 2121%'
				OR `dae_course`.`course` LIKE '%csci 2121%'

		)
)
