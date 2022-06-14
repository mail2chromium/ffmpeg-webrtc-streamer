// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCATION_REPORT_BODY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCATION_REPORT_BODY_H_

#include <memory>
#include <utility>

#include "third_party/blink/renderer/bindings/core/v8/source_location.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"
#include "third_party/blink/renderer/core/frame/report_body.h"

namespace blink {

class CORE_EXPORT LocationReportBody : public ReportBody {
 private:
  struct ReportLocation {
    String file;
    base::Optional<uint32_t> line_number;
    base::Optional<uint32_t> column_number;
  };

  static ReportLocation CreateReportLocation(
      const String& file,
      base::Optional<uint32_t> line_number,
      base::Optional<uint32_t> column_number);

  static ReportLocation CreateReportLocation(
      std::unique_ptr<SourceLocation> location);

  explicit LocationReportBody(const ReportLocation& location)
      : source_file_(location.file),
        line_number_(location.line_number),
        column_number_(location.column_number) {}

 protected:
  explicit LocationReportBody(std::unique_ptr<SourceLocation> location)
      : LocationReportBody(CreateReportLocation(std::move(location))) {}

  explicit LocationReportBody(
      const String& source_file = g_empty_string,
      base::Optional<uint32_t> line_number = base::nullopt,
      base::Optional<uint32_t> column_number = base::nullopt)
      : LocationReportBody(
            CreateReportLocation(source_file, line_number, column_number)) {}

 public:
  ~LocationReportBody() override = default;

  const String& sourceFile() const { return source_file_; }

  base::Optional<uint32_t> lineNumber() const { return line_number_; }
  base::Optional<uint32_t> columnNumber() const { return column_number_; }

  void BuildJSONValue(V8ObjectBuilder& builder) const override;

 protected:
  const String source_file_;
  const base::Optional<uint32_t> line_number_;
  const base::Optional<uint32_t> column_number_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCATION_REPORT_BODY_H_
