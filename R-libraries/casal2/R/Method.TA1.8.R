#' @title Method.TA1.8
#' @description 
#' This function is useful for deciding on the data weights of one or more at-age or at-length data sets with assumed multinomial error structure in a stock assessment. Can produce a diagnostic plot if the analysis is for a single data set
#'
#' @author Chris Francis
#' @param model Casal2 output that is the result of a -r, -e run.
#' @param observation_labels vector<string> Labels of the observations you want to apply the iterative weighting too, can be multiple datasets as in in Chris's original package multiple = T.
#' @param plot.it If TRUE, plot the index and the smoothed fit. Otherwise, return a dataframe of the year, index, smoothed fitted value, and cv)
#' @param ylim y-axis limits for the illustrative plot
#' @param xlim x-axis limits for the illustrative plot
#'
#' @return Outputs a mutiplier, w, so that N2y = w x N1y, where N1y and N2y are the stage-1 and stage-2 multinomial sample sizes for the data set in year y.
#'
#' @note Method TA1.8 is described in Appendix A of the following paper Francis, R.I.C.C. (2011). Data weighting in statistical fisheries stock assessment models. 
#' Canadian Journal of Fisheries and Aquatic Sciences 68: 1124-1138. (With corrections to the equation in Francis R.I.C.C. (2011) Corrigendum: Data weighting in statistical fisheries stock assessment models. 
#' @export

 Method.TA1.8 = function (model, observation_labels, plot.it = F, xlim = NULL, ylim = NULL) {
  ############################
  ## Validate input parameters
  ############################
  multiple = FALSE;
  transformed_data = list();
  if (length(observation_labels) > 1)
    multiple = TRUE;
  for (i in 1:length(observation_labels)) {
    ## check report label exists
    if (!observation_labels[i] %in% names(model))
      stop(Paste("In model the report label '", observation_labels[i], "' could not be found. The report labels available are ", paste(names(model),collapse = ", ")))
    ## get the report out
    this_report = get(observation_labels[i], model)
    ## check that the Observation_label is of type observation
    if (this_report$'1'$type != "observation") {
      stop(Paste("The report label ", observation_labels[i], " in model is not a observation report, it is a ",this_report$'1'$type," report, plz Check you have specified the correct Observation_label"))     
    }
    if (length(this_report) > 1) {
        stop(Paste("Found: ",length(this_report) ," reports in under the observation_label, ", observation_labels[i],", ususally this is caused by a multiline parameter run in casal2 -i or a profile run casal2 -p. This function will not work for those run types"))
    }
    ## Reformat the obs
    Data = this_report$'1'$Values
    years = unique(Data[,"year"])
    n_categories = length(unique(Data[,"category"]))

    if (n_categories > 2) {
      stop(Paste("This function can currently only deal with an age composition with 2 unique categories, but we found ", n_categories, " plz check you have selected the correct observation to weight"))
    }
    ages = unique(Data[,"age"])
    lengths = unique(Data[,"length"])
    if (length(ages) > 1 & length(lengths) > 1) {
      stop("This observation contains both length and age bins. something is wrong.")
    }
    bins = 0
    if (length(ages) > 1) {
      bins = ages
    } else {
      bins = lengths;
    }
    n_bins = length(bins)  
    n_years = length(years)

    obs = matrix(Data[, "observed"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
    fit = matrix(Data[, "expected"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
    err = matrix(Data[, "error_value"], byrow = T, ncol = n_bins * n_categories, nrow = n_years)
    rownames(fit) = rownames(err) = rownames(obs) = years
    if(n_categories == 1) {
      colnames(obs) = Paste(unique(Data[,"category"]), "[",bins,"]")
      colnames(fit) = Paste(unique(Data[,"category"]), "[",bins,"]")
      colnames(err) = Paste(unique(Data[,"category"]), "[",bins,"]")    
    } else {
      colnames(obs) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))
      colnames(fit) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))
      colnames(err) = c(Paste(unique(Data[,"category"])[1], "[",bins,"]"),Paste(unique(Data[,"category"])[2], "[",bins,"]"))      
    }
    transformed_data[[observation_labels[i]]]$obs = obs
    transformed_data[[observation_labels[i]]]$fits = fit
    transformed_data[[observation_labels[i]]]$error.value = err    
    transformed_data[[observation_labels[i]]]$year = years    
    
  }
  ###########################
  ## Execute the function.
  ###########################
  
  ## reformat the data so that we can use Chris's code.
  if (plot.it & multiple) 
      stop("Can't produce diagnostic plot for multiple data sets")
  
  Ndat <- length(observation_labels)
  Nassumed <- Ry <- Sy <- c();
  for (i in 1:Ndat) {
      
      thisfit <- transformed_data[[i]]
      if (length(thisfit$obs) == 0) 
          stop("Can't find observations")
      Obs <- as.matrix(thisfit$obs)
      Obs <- sweep(Obs, 1, apply(Obs, 1, sum), "/")
      Exp <- as.matrix(thisfit$fit)
      Exp <- sweep(Exp, 1, apply(Exp, 1, sum), "/")
      Nassumed <- c(Nassumed, apply(as.matrix(thisfit$error.value), 
          1, mean))
      years <- as.numeric(dimnames(Obs)[[1]])
      ## pull out bins
      first_char = regexpr("\\[", dimnames(Obs)[[2]], fixed = F, useBytes = F)
      second_char = regexpr("]", dimnames(Obs)[[2]], fixed = F, useBytes = F)
      aa = as.numeric(substr(dimnames(Obs)[[2]], start = first_char + 1, stop = second_char - 1))
      ##aa <- as.numeric(strsplit(dimnames(Obs)[[2]],split = "\\["), nchar(dimnames(Obs)[[2]])))
      My <- cbind(Obs = apply(Obs, 1, function(x) sum(aa * 
          x)), Exp = apply(Exp, 1, function(x) sum(aa * x)))
      Ry <- c(Ry, My[, "Obs"] - My[, "Exp"])
      Sy <- c(Sy, sqrt(apply(Exp, 1, function(x) sum(x * aa^2)) - 
          My[, "Exp"]^2))
  }
  wj <- 1/var(Ry * sqrt(Nassumed)/Sy, na.rm = T)
  if (plot.it) {
      ses <- Sy/sqrt(Nassumed)
      Obs.bnds <- My[, "Obs"] + cbind(-2 * ses, 2 * ses)
      if (is.null(ylim)) 
          ylim <- range(Obs.bnds)
      if (is.null(xlim)) 
          xlim <- range(years)
      plot(years, My[, "Obs"], type = "n", ylab = "", xlab = "", 
          xlim = xlim, ylim = ylim, las = 1)
      points(years, My[, "Obs"], pch = "x", col = 3)
      segments(years, Obs.bnds[, 1], years, Obs.bnds[, 2], 
          col = 3)
      lines(years, My[, "Exp"], col = 2)
  }
  wj
}
