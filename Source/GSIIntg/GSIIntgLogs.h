// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GSIINTG_API DECLARE_LOG_CATEGORY_EXTERN(LogGSII, Log, All);

#if !UE_BUILD_SHIPPING

#define GSIILOG(FormattedText, ...) UE_LOG(LogGSII, Log, FormattedText, __VA_ARGS__)

#define GSIIENSURE(InExpression) ensure(InExpression)
#define GSIIENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GSIIENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GSIICHECK(InExpression) check(InExpression)
#define GSIICHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GSIILOG(FormattedText, ...)

#define GSIIENSURE(InExpression) InExpression
#define GSIIENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GSIIENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GSIICHECK(InExpression) InExpression
#define GSIICHECK_MSG(InExpression, InFormat, ...) InExpression

#endif