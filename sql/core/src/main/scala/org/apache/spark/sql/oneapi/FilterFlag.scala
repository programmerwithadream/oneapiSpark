
package org.apache.spark.sql.oneapi

import org.apache.spark.sql.execution.{FilterExec, SparkPlan}

object FilterFlag {

  var value: Boolean = false

  def hasFilterExec(plan: SparkPlan): Boolean = {
    plan match {
      case _: FilterExec => true
      case plan: SparkPlan =>
        val lambda = child => hasFilterExec(child)
        plan.children.exists(lambda)
      case _ => false
    }
  }

}